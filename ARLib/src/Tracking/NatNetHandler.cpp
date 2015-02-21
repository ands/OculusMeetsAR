#include "ARLib/Tracking/NatNetHandler.h"
#include "NatNetClient.h"
#include <Windows.h>

namespace ARLib{

const std::string NatNetHandler::invalidIP = "-1.-1.-1.-1";

NatNetHandler::NatNetHandler(ConnectionType iCType, std::string logFile)
	: mConnectionState(NATNET_PENDING){
	mClientHandle = new NatNetClient(iCType);

	mClientHandle->SetMessageCallback(MessageHandler);
	mClientHandle->SetVerbosityLevel(Verbosity_Info);
	mClientHandle->SetDataCallback(NatNetHandler::DataHandler, this);
}

NatNetHandler::~NatNetHandler(){
	disconnect();
	delete mClientHandle;
}

int NatNetHandler::connect(const char* rClientIP, const char* rServerIP){
	if (mConnectionState == NATNET_CONNECTED){
		disconnect();
	}

	int ret = mClientHandle->Initialize(const_cast<char*>(rClientIP), const_cast<char*>(rServerIP));

	if (ret != ErrorCode_OK){
		return ErrorCode_Internal;
	}else{
		mConnectionState = NATNET_CONNECTED;
	}

	mServerIP = rServerIP;
	mClientIP = rClientIP;

	//gather Server info
	sServerDescription ServerDescription;
	memset(&ServerDescription, 0, sizeof(ServerDescription));
	mClientHandle->GetServerDescription(&ServerDescription);

	if (!ServerDescription.HostPresent){
		mConnectionState = NATNET_DISCONNECTED;
		return 1;
	}
	return ret;
}

int NatNetHandler::disconnect(){
	mConnectionState = NATNET_DISCONNECTED;
	return mClientHandle->Uninitialize();
}
	
CONNECTION_STATE NatNetHandler::connected()const{
	return mConnectionState;
}

const std::string& NatNetHandler::getServerIP() const{
	if(mConnectionState != NATNET_CONNECTED)
		return invalidIP;
	return mServerIP;
}

const std::string& NatNetHandler::getClientIP() const{
	if(mConnectionState != NATNET_CONNECTED)
		return invalidIP;
	return mClientIP;
}

void NatNetHandler::registerFrameEvaluator(GenericNatNetEvaluator* evaluator){
	mEvaluator = evaluator;
}

double NatNetHandler::getPing() const{
    LARGE_INTEGER start, end, freq, elapsed;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    int nbytes = 0;
    void* response;
    mClientHandle->SendMessageAndWait("ping", 1, 50, &response, &nbytes);
    QueryPerformanceCounter(&end);
    elapsed.QuadPart = (end.QuadPart - start.QuadPart)*1000000;
    elapsed.QuadPart /= freq.QuadPart;
    double time = static_cast<double>(elapsed.QuadPart)/1000.0;
    return time;
}

void NatNetHandler::MessageHandler(int iId, char* pMsg){

}

void NatNetHandler::DataHandler(sFrameOfMocapData *pFrame, void *pClient){
	NatNetHandler *pHandler = (NatNetHandler*) pClient;
    double serverLatency = pHandler->getPing();
   
	bool change = true;//(bool)(pFrame->params & 0x02);
	if(!change){
		return;
	}
	RBFrame *frame = new RBFrame(pFrame->nRigidBodies, pFrame->iFrame, pFrame->fTimestamp, pFrame->fLatency, true, true);
	frame->mChange = change;

	for(int i = 0; i < pFrame->nRigidBodies; i++){
		sRigidBodyData& pRB = pFrame->RigidBodies[i];
		RigidBody *rb = new RigidBody(pRB.nMarkers);
		rb->mVisible = pRB.params & 0x01;
		rb->mID = pRB.ID;
		rb->mError = pRB.MeanError;
		rb->mX = pRB.x;
		rb->mY = pRB.y;
		rb->mZ = pRB.z;
		rb->mqX = pRB.qx;
		rb->mqY = pRB.qy;
		rb->mqZ = pRB.qz;
		rb->mqW = pRB.qw;
		for(unsigned int j = 0; j < rb->mNMarker; j++){
			int markerID = pRB.MarkerIDs ? pRB.MarkerIDs[j] : -1;
			float markerSize = pRB.MarkerSizes ? pRB.MarkerSizes[j] : 0.0f;
			float x = pRB.Markers ? pRB.Markers[j][0] : 0.0f;
			float y = pRB.Markers ? pRB.Markers[j][1] : 0.0f;
			float z = pRB.Markers ? pRB.Markers[j][2] : 0.0f;
			rb->addMarker(j, markerID, markerSize, x, y, z);
		}
		frame->addRigidBody(i, rb);
	}

	if(pHandler->mEvaluator){
		pHandler->mEvaluator->updateFrame(frame);
	}
}

};