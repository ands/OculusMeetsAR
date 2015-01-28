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
#include <string>

class NatNetClient;

namespace ARLib{

	typedef enum CONNECTION_STATE{
		NATNET_CONNECTED = 0,	//all ok!
		NATNET_PENDING,			//before first connection
		NATNET_DISCONNECTED		//after first loss of connection
	}CONNECTION_STATE;


	class NatNetHandler{
	public:
		NatNetHandler(ConnectionType iCType, std::string logFile = "NatNetHandler.info");
		~NatNetHandler();

		int connect(const char* rClientIP, const char* rServerIP);
		int disconnect();

		CONNECTION_STATE connected()const;

		const std::string& getServerIP() const;
		const std::string& getClientIP() const;

		void registerFrameEvaluator(FrameEvaluator* evaluator);
        double getPing()const;
	private:
		static const std::string invalidIP;

		static void MessageHandler(int iId, char* pMsg);
		static void DataHandler(sFrameOfMocapData *pFrame, void *pClient);

		std::string mServerIP;
		std::string mClientIP;
		int mConnectionType; //Multicast or Unicast

		sDataDescriptions *mDataDesc;
		NatNetClient *mClientHandle;
		FrameEvaluator *mEvaluator;

		CONNECTION_STATE mConnectionState;
	};

};
#endif
