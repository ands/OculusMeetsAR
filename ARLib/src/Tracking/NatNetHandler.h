#ifndef NAT_NET_CLIENT_H
#define NAT_NET_CLIENT_H

/**************************************************
This Class Manages the handle of the NatNetClient and provides access to the 
Motive stream data more easily.
**************************************************/

#include "../General/InfoLog.h"
#include "TrackingDll.h"
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include <string>

typedef enum{
	NATNET_CONNECTED = 0,	//all ok!
	NATNET_PENDING,			//before first connection
	NATNET_DISCONNECTED		//after first loss of connection
}Connection_State;

class TRACKINGDLL_API NatNetHandler{
public:
	NatNetHandler(int iCType, const std::string& logFile = "NatNetHandler.info");
	~NatNetHandler();

	int connect(char* rClientIP, char* rServerIP);
	int disconnect();
private:
	void MessageHandler(int iId, char* pMsg);
	void DataHandler(sFrameOfMocapData *pFrame, void *pUserData);


	char cServerIP[4];
	char cClientIP[4];
	int iConnectionType; //Multicast or Unicast

	sDataDescriptions *pDataDesc;
	NatNetClient *pClientHandle;
	Connection_State eConnectionState;

	InfoLog *pNatNetInfoLog;
};

#endif
