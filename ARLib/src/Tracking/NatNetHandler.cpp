#include "Tracking\NatNetHandler.h"
#include <WS2tcpip.h>

namespace ARLib{

NatNetHandler::NatNetHandler(ConnectionType iCType, std::string logFile)
	: mConnectionState(NATNET_PENDING){
	mClientHandle = new NatNetClient(iCType);

	mClientHandle->SetMessageCallback(MessageHandler);
	mClientHandle->SetVerbosityLevel(Verbosity_Info);
	mClientHandle->SetDataCallback(DataHandler, this);
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

	//gather ip addresses
	struct sockaddr_in sa;
	inet_pton(AF_INET, rClientIP, &(sa.sin_addr));
	ULONG ulIp = sa.sin_addr.S_un.S_addr;
	mClientIP.byte[0] = ulIp & 0xFF;
	mClientIP.byte[1] = (ulIp >> 8) & 0xFF;
	mClientIP.byte[2] = (ulIp >> 16) & 0xFF;
	mClientIP.byte[3] = (ulIp >> 24) & 0xFF;

	inet_pton(AF_INET, rServerIP, &(sa.sin_addr));
	ulIp = sa.sin_addr.S_un.S_addr;
	mServerIP.byte[0] = ulIp & 0xFF;
	mServerIP.byte[1] = (ulIp >> 8) & 0xFF;
	mServerIP.byte[2] = (ulIp >> 16) & 0xFF;
	mServerIP.byte[3] = (ulIp >> 24) & 0xFF;

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

ipAddress NatNetHandler::getServerIP() const{
	if(mConnectionState != NATNET_CONNECTED){
		ipAddress dummy;
		dummy.byte[0] = 0;
		dummy.byte[1] = 0;
		dummy.byte[2] = 0;
		dummy.byte[3] = 0;
		return dummy;
	}
	return mServerIP;
}

ipAddress NatNetHandler::getClientIP() const{
	if(mConnectionState != NATNET_CONNECTED){
		ipAddress dummy;
		dummy.byte[0] = 0;
		dummy.byte[1] = 0;
		dummy.byte[2] = 0;
		dummy.byte[3] = 0;
		return dummy;
	}
	return mClientIP;
}

void NatNetHandler::registerFrameEvaluator(FrameEvaluator* evaluator){
	mEvaluator = evaluator;
}

void NatNetHandler::MessageHandler(int iId, char* pMsg){

}

void NatNetHandler::DataHandler(sFrameOfMocapData *pFrame, void *pClient){
	NatNetHandler *pHandler = (NatNetHandler*) pClient;

	bool change = (bool)(pFrame->params & 0x02);
	if(!change){
		return;
	}

	RBFrame *frame = new RBFrame(pFrame->nRigidBodies, pFrame->iFrame, pFrame->fTimestamp, pFrame->fLatency);
	frame->mChange = change;

	for(int i = 0; i < pFrame->nRigidBodies; i++){
		RigidBody *rb = new RigidBody();
		sRigidBodyData& pRB = pFrame->RigidBodies[i];
		rb->mVisible = pRB.params & 0x01;
		rb->mID = pRB.ID;
		rb->mError = pRB.MeanError;
		rb->mPosition = Ogre::Vector3(pRB.x, pRB.y, pRB.z);
		rb->mOrientation = Ogre::Quaternion(pRB.qx, pRB.qy, pRB.qz, pRB.qw);
		rb->mNMarker = pRB.nMarkers;
		rb->mMarkers = new Marker[rb->mNMarker];

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