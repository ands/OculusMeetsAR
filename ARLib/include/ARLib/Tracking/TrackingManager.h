#ifndef ARLIB_TRACKING_MANAGER_H
#define ARLIB_TRACKING_MANAGER_H

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/
#include <string>
#include "FrameEvaluator.h"

typedef enum ConnectionType ConnectionType;

namespace ARLib{

	typedef enum FRAME_EVALUATION_METHOD;

	typedef enum{
		ARLIB_NATNET = 0x01,
		ARLIB_RIFT = 0x02,
	}TRACKING_METHOD;

	typedef enum{
		NONE = 0x00,
		ARLIB_TRACKING_OK = 0x01,
		ARLIB_TRACKING_NATNET_ERROR = 0x02,
		ARLIB_TRACKING_RIFT_ERROR = 0x04,
		ARLIB_TRACKING_NO_DEVICE_ERROR = 0x08
	}TRACKING_ERROR_CODE;

	typedef struct _RigidBody RigidBody;
	class RigidBodyEventListener;
	class NatNetHandler;
	class Rift;
	class FrameEvaluator;

	class TrackingManager{
	public:
		TrackingManager(TRACKING_METHOD tracking, unsigned int frameBufferSize, Rift *oculusHMD = nullptr);
		~TrackingManager();
        
		TRACKING_ERROR_CODE initialize(); 
		void uninitialize();
		TRACKING_ERROR_CODE reinitialize(); 

        RigidBody *evaluateRigidBody(unsigned int ID, const long long& retroActiveQuery);
		void update();

		void setNatNetConnectionType(ConnectionType cType);
		void setNatNetServerIP(const std::string& sIP = "");
		void setNatNetClientIP(const std::string& cIP = "");
		void setNatNetHostCommandPort(int hostCommandPort);
		void setNatNetHostDataPort(int hostDataPort);
        void setFrameEvaluationMethod(FRAME_EVALUATION_METHOD eval);

		void addRigidBodyEventListener(RigidBodyEventListener* listener);
	private:
		bool mInitialized;
		unsigned int mFrameBufferSize;

		FRAME_EVALUATION_METHOD mEval;
		TRACKING_METHOD mTracking;
		Rift *mRiftHandle;
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
