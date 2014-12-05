#include "ARLib/Tracking/NatNetHandler.h"
#include "NatNetClient.h"

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
		//pNatNetInfoLog->log("Unable to connect to server. Error Code " + std::to_string((long long)ret) + "\n");
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
		//pNatNetInfoLog->log("Unable to connect to server. Host not present. Disconnecting.\n");
		mConnectionState = NATNET_DISCONNECTED;
		return 1;
	}
	//pNatNetInfoLog->log("Successfully connected to Server.\n");
	//pNatNetInfoLog->log("Application: " + std::string(ServerDescription.szHostApp) + " (version" +
	//	std::to_string((long long)ServerDescription.HostAppVersion[0]) + "." + std::to_string((long long)ServerDescription.HostAppVersion[1]) + "." + 
	//	std::to_string((long long)ServerDescription.HostAppVersion[2]) + "." + std::to_string((long long)ServerDescription.HostAppVersion[3]) + ")" );
	//pNatNetInfoLog->log("NatNet Version : ");
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

void NatNetHandler::registerFrameEvaluator(FrameEvaluator* evaluator){
	mEvaluator = evaluator;
}

void NatNetHandler::MessageHandler(int iId, char* pMsg){

}

void NatNetHandler::DataHandler(sFrameOfMocapData *pFrame, void *pClient){
	NatNetHandler *pHandler = (NatNetHandler*) pClient;

	bool change = true;//(bool)(pFrame->params & 0x02);
	if(!change){
		return;
	}
	RBFrame *frame = new RBFrame(pFrame->nRigidBodies, pFrame->iFrame, pFrame->fTimestamp, pFrame->fLatency);
	frame->mChange = change;

	for(int i = 0; i < pFrame->nRigidBodies; i++){
		sRigidBodyData& pRB = pFrame->RigidBodies[i];
		RigidBody *rb = new RigidBody(pRB.nMarkers);
		rb->mVisible = pRB.params & 0x01;
		rb->mID = pRB.ID;
		rb->mError = pRB.MeanError;
		rb->mPosition = Ogre::Vector3(pRB.x, pRB.y, pRB.z);
		rb->mOrientation = Ogre::Quaternion(pRB.qx, pRB.qy, pRB.qz, pRB.qw);

		for(int j = 0; j < rb->mNMarker; j++){
			int markerID = pRB.MarkerIDs ? pRB.MarkerIDs[j] : -1;
			float markerSize = pRB.MarkerSizes ? pRB.MarkerSizes[j] : 0.0f;
			Ogre::Vector3 markerPosition = pRB.Markers ? 
										Ogre::Vector3(pRB.Markers[j][0],
													  pRB.Markers[j][1],
													  pRB.Markers[j][2]) : Ogre::Vector3();
			rb->addMarker(j, markerID, markerSize, markerPosition);
		}
		frame->addRigidBody(i, rb);
	}

	if(pHandler->mEvaluator){
		pHandler->mEvaluator->updateFrame(frame);
	}
}

};