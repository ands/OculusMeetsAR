#ifndef ARLIB_NAT_NET_HANDLER_H
#define ARLIB_NAT_NET_HANDLER_H

/**************************************************
This Class Manages the handle of the NatNetClient and provides access to the 
Motive stream data more easily.
**************************************************/

#include "ARLib/Tracking/RigidBodyFrame.h"
#include "ARLib/Tracking/FrameEvaluator.h"
#include "ARLib/General/InfoLog.h"
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include <string>

namespace ARLib{

	typedef enum{
		NATNET_CONNECTED = 0,	//all ok!
		NATNET_PENDING,			//before first connection
		NATNET_DISCONNECTED		//after first loss of connection
	}CONNECTION_STATE;

	typedef struct{
		union{	
			struct{
				char one;
				char two;
				char three;
				char four;
			};
			char byte[4];
		};
	}ipAddress;

	class NatNetHandler{
	public:
		NatNetHandler(ConnectionType iCType, std::string logFile = "NatNetHandler.info");
		~NatNetHandler();

		int connect(const char* rClientIP, const char* rServerIP);
		int disconnect();

		CONNECTION_STATE connected()const;

		ipAddress getServerIP() const;
		ipAddress getClientIP() const;

		void registerFrameEvaluator(FrameEvaluator* evaluator);
	private:
		static void MessageHandler(int iId, char* pMsg);
		static void DataHandler(sFrameOfMocapData *pFrame, void *pClient);

		ipAddress mServerIP;
		ipAddress mClientIP;
		int mConnectionType; //Multicast or Unicast

		sDataDescriptions *mDataDesc;
		NatNetClient *mClientHandle;
		FrameEvaluator *mEvaluator;

		CONNECTION_STATE mConnectionState;
	};

};
#endif
