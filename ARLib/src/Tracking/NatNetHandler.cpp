#include "NatNetHandler.h"
#include <WS2tcpip.h>

NatNetHandler::NatNetHandler(int iCType, const std::string& logFile)
	: eConnectionState(NATNET_PENDING){
	pClientHandle = new NatNetClient(iCType);

	pClientHandle->SetMessageCallback(this->MessageHandler);
	pClientHandle->SetVerbosityLevel(Verbosity_Info);
	pClientHandle->SetDataCallback(this->DataHandler, pClientHandle);

	pNatNetInfoLog = new InfoLog(logFile);
}

NatNetHandler::~NatNetHandler(){
	delete pNatNetInfoLog;
}

int NatNetHandler::connect(char* rClientIP, char* rServerIP){
	if (eConnectionState == NATNET_CONNECTED){
		disconnect();
		eConnectionState = NATNET_DISCONNECTED;
	}

	int ret = pClientHandle->Initialize(rClientIP, rServerIP);

	if (ret != ErrorCode_OK){
		pNatNetInfoLog->log("Unable to connect to server. Error Code " + std::to_string(ret) + "\n");
		return ErrorCode_Internal;
	}else{
		eConnectionState = NATNET_CONNECTED;
	}

	//gather ip addresses
	struct sockaddr_in sa;
	inet_pton(AF_INET, rClientIP, &(sa.sin_addr));
	ULONG ulIp = sa.sin_addr.S_un.S_addr;
	cClientIP[0] = ulIp & 0xFF;
	cClientIP[1] = (ulIp >> 8) & 0xFF;
	cClientIP[2] = (ulIp >> 16) & 0xFF;
	cClientIP[3] = (ulIp >> 24) & 0xFF;

	inet_pton(AF_INET, rServerIP, &(sa.sin_addr));
	ulIp = sa.sin_addr.S_un.S_addr;
	cServerIP[0] = ulIp & 0xFF;
	cServerIP[1] = (ulIp >> 8) & 0xFF;
	cServerIP[2] = (ulIp >> 16) & 0xFF;
	cServerIP[3] = (ulIp >> 24) & 0xFF;

	//gather Server info
	sServerDescription ServerDescription;
	memset(&ServerDescription, 0, sizeof(ServerDescription));
	pClientHandle->GetServerDescription(&ServerDescription);

	if (!ServerDescription.HostPresent){
		pNatNetInfoLog->log("Unable to connect to server. Host not present. Disconnecting.\n");
		eConnectionState = NATNET_DISCONNECTED;
		return 1;
	}

	pNatNetInfoLog->log("Successfully connected to Server.\n");
	pNatNetInfoLog->log("Application: " + std::string(ServerDescription.szHostApp) + " (version" +
		std::to_string(ServerDescription.HostAppVersion[0]) + "." + std::to_string(ServerDescription.HostAppVersion[1]) + "." + 
		std::to_string(ServerDescription.HostAppVersion[2]) + "." + std::to_string(ServerDescription.HostAppVersion[3]) + ")" );
	pNatNetInfoLog->log("NatNet Version : ");
	return ret;
}

int NatNetHandler::disconnect(){

}

void NatNetHandler::MessageHandler(int iId, char* pMsg){
	pNatNetInfoLog->log(std::to_string(iId) + ":\t" + pMsg + "\n");
}

void NatNetHandler::DataHandler(sFrameOfMocapData *pFrame, void *pUserData){

}