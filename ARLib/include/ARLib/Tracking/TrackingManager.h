#ifndef ARLIB_TRACKING_MANAGER_H
#define ARLIB_TRACKING_MANAGER_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/
#include <string>
#include "FrameEvaluator.h"

enum ConnectionType;

namespace ARLib{

	enum FRAME_EVALUATION_METHOD;

	enum TRACKING_METHOD{
		ARLIB_NATNET = 0x01,
		ARLIB_RIFT = 0x02,
	};

	enum TRACKING_ERROR_CODE{
		NONE = 0x00,
		ARLIB_TRACKING_OK = 0x01,
		ARLIB_TRACKING_NATNET_ERROR = 0x02,
		ARLIB_TRACKING_RIFT_ERROR = 0x04,
		ARLIB_TRACKING_NO_DEVICE_ERROR = 0x08
	};

	struct RigidBody;
	class RigidBodyEventListener;
	class NatNetHandler;
	class Rift;
	class FrameEvaluator;

	/**********************************************************************************************
	Interface for Tracking Hardware such as Oculus Rift (DK1 and DK2) and any Tracking Server based
	on the NatNet Software Development Kit.
	**********************************************************************************************/
	class TrackingManager{
	public:
		TrackingManager(TRACKING_METHOD tracking, unsigned int frameBufferSize, bool DebugOutput);
		~TrackingManager();
        
		//initializes the handles for tracking, thereby establishing a connection to a natnet server and or checking the availability of a rift
		TRACKING_ERROR_CODE initialize(); 
		//uninitialize the handles for tracking
		void uninitialize();
		//quick and fast uninitialize & initialize function
		TRACKING_ERROR_CODE reinitialize(); 

		//Retrieve the RigidBody with a certain ID at a particular Point in Time
        RigidBody *evaluateRigidBody(unsigned int ID, const long long& retroActiveQuery);

		//Update all RigidBodyEventListeners i.e. call their onChange function if necessary
		void update();
		
		void setNatNetConnectionType(ConnectionType cType);
		void setNatNetServerIP(const std::string& sIP = "");
		void setNatNetClientIP(const std::string& cIP = "");
		void setNatNetHostCommandPort(int hostCommandPort);
		void setNatNetHostDataPort(int hostDataPort);

		//set the Frameevaluation method, used during a call of evaluateRigidBody
        void setFrameEvaluationMethod(FRAME_EVALUATION_METHOD eval);
		//adds a RigidBodyEventListener. Call this function only after a successful initialize!
		void addRigidBodyEventListener(RigidBodyEventListener* listener);
	private:
		bool mInitialized;
		bool mDebugOutput;
		unsigned int mFrameBufferSize;

		FRAME_EVALUATION_METHOD mEval;
		TRACKING_METHOD mTracking;
		FrameEvaluator *mEvaluator;

		int mNatNetHostCommandPort;
		int mNatNetHostDataPort;
		std::string mNatNetServerIP;
		std::string mNatNetClientIP;
		ConnectionType mNatNetConnectionType;

		NatNetHandler *mNatNetHandler;
	};

	inline TRACKING_METHOD operator| (const TRACKING_METHOD& lhs, const TRACKING_METHOD& rhs){
		return static_cast<TRACKING_METHOD>( static_cast< int >( lhs ) | static_cast< int >( rhs ) );
	}

	inline TRACKING_ERROR_CODE operator| (const TRACKING_ERROR_CODE& lhs, const TRACKING_ERROR_CODE& rhs){
		return static_cast<TRACKING_ERROR_CODE>( static_cast< int >( lhs ) | static_cast< int >( rhs ) );
	}

	inline TRACKING_ERROR_CODE operator& (const TRACKING_ERROR_CODE& lhs, const TRACKING_ERROR_CODE& rhs){
		return static_cast<TRACKING_ERROR_CODE>( static_cast< int >( lhs ) & static_cast< int >( rhs ) );
	}
};
#endif
