#include "Tracking/TrackingManager.h"
#include "Tracking/RigidBodyEventListener.h"

namespace ARLib{
	
TrackingManager::TrackingManager(TRACKING_METHOD tracking, Rift *oculusHMD)
	: mTracking(tracking)
	, mRiftHandle(oculusHMD){
	mEvaluator = new FrameEvaluator();
}

TrackingManager::~TrackingManager(){
	if(mNatNetHandler)
		delete mNatNetHandler;
	delete mEvaluator;
}

TRACKING_ERROR_CODE TrackingManager::initialize(){
	if(!(mTracking & (ARLIB_NATNET | ARLIB_RIFT))){
		return ARLIB_TRACKING_NO_DEVICE_ERROR;
	}
	if(mTracking & ARLIB_NATNET){
		mNatNetHandler = new NatNetHandler(mNatNetConnectionType);
		mNatNetHandler->registerFrameEvaluator(mEvaluator);
		mNatNetHandler->connect(mNatNetServerIP.c_str(), mNatNetClientIP.c_str());
		if(mNatNetHandler->connected() & NATNET_DISCONNECTED ||
			mNatNetHandler->connected() & NATNET_PENDING){
				return ARLIB_TRACKING_NATNET_ERROR;
		}
	}
	if(mTracking & ARLIB_RIFT){
		if(!mRiftHandle){
			return ARLIB_TRACKING_RIFT_ERROR;
		}
	}
	return ARLIB_TRACKING_OK;	
}
		
void TrackingManager::setRiftRigidBodyID(unsigned int id){

}

void setNatNetConnectionType(ConnectionType cType){

}
		
void TrackingManager::setNatNetServerIP(const std::string& sIP){

}

void TrackingManager::setNatNetClientIP(const std::string& cIP){

}

void TrackingManager::registerRigidBodyEventListener(RigidBodyEventListener* listener){
	mRigidBodies.push_back(listener);
}

};