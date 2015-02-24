#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/Tracking/FrameEvaluator.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"
#include "ARLib/Tracking/NatNetHandler.h"
#include "ARLib/Oculus/Rift.h"

namespace ARLib{
	
TrackingManager::TrackingManager(TRACKING_METHOD tracking, unsigned int frameBufferSize, Rift *oculusHMD)
	: mTracking(tracking)
	, mRiftHandle(oculusHMD)
	, mEvaluator(nullptr)
	, mNatNetHandler(nullptr)
	, mFrameBufferSize(frameBufferSize)
	, mInitialized(false)
	, mNatNetHostCommandPort(1510)
	, mNatNetHostDataPort(1511)
	, mEval(FRAME_ROUND){

}

TrackingManager::~TrackingManager(){
	if(mNatNetHandler != nullptr)
		delete mNatNetHandler;
	delete mEvaluator;
}

TRACKING_ERROR_CODE TrackingManager::initialize(){
	TRACKING_ERROR_CODE errorCode = NONE;
	if((mTracking == (ARLIB_NATNET | ARLIB_RIFT))){
		delete mEvaluator;
		mEvaluator = new NatNetRiftEvaluator(mFrameBufferSize);
		mNatNetHandler = new NatNetHandler(mNatNetConnectionType);
		mNatNetHandler->registerFrameEvaluator(dynamic_cast<GenericNatNetEvaluator*>(mEvaluator));
		mNatNetHandler->connect(mNatNetServerIP.c_str(), mNatNetClientIP.c_str(), mNatNetHostCommandPort, mNatNetHostDataPort);
		if(mNatNetHandler->connected() & NATNET_DISCONNECTED ||
			mNatNetHandler->connected() & NATNET_PENDING){
				errorCode = errorCode | ARLIB_TRACKING_NATNET_ERROR;
		}
	}else if(mTracking & ARLIB_NATNET){
		delete mEvaluator;
		mEvaluator = new NatNetEvaluator(mFrameBufferSize);
		mNatNetHandler = new NatNetHandler(mNatNetConnectionType);
		mNatNetHandler->registerFrameEvaluator(dynamic_cast<GenericNatNetEvaluator*>(mEvaluator));
		mNatNetHandler->connect(mNatNetServerIP.c_str(), mNatNetClientIP.c_str(), mNatNetHostCommandPort, mNatNetHostDataPort);
		if(mNatNetHandler->connected() & NATNET_DISCONNECTED ||
			mNatNetHandler->connected() & NATNET_PENDING){
				errorCode = errorCode | ARLIB_TRACKING_NATNET_ERROR;
		}
	}else if(mTracking & ARLIB_RIFT){
		delete mEvaluator;
		mEvaluator = new RiftEvaluator(mFrameBufferSize);
		if(!mRiftHandle){
			errorCode = errorCode | ARLIB_TRACKING_RIFT_ERROR;
		}
	}
	if(mEvaluator != nullptr){
		mEvaluator->setEvaluationMethod(mEval);
	}
	if(errorCode == NONE){
		mInitialized = true;
		return ARLIB_TRACKING_OK;	
	}else{
		mInitialized = false;
		return errorCode;
	}
}

void TrackingManager::uninitialize(){
	if(mNatNetHandler != nullptr){
		delete mNatNetHandler;
		mNatNetHandler = nullptr;
	}
	mInitialized = false;
}

TRACKING_ERROR_CODE TrackingManager::reinitialize(){
    uninitialize();
    return initialize();
}
		
RigidBody *TrackingManager::evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime){
	if(mInitialized){
        return mEvaluator->evaluateRigidBody(ID, retroActiveQueryTime);
	}
    return nullptr;
}

void TrackingManager::update(){
	if(mInitialized){
		mEvaluator->evaluate();
	}
}

void TrackingManager::setNatNetConnectionType(ConnectionType cType){
	mNatNetConnectionType = cType;
}
		
void TrackingManager::setNatNetServerIP(const std::string& sIP){
	mNatNetServerIP = sIP;
}

void TrackingManager::setNatNetClientIP(const std::string& cIP){
	mNatNetClientIP = cIP;
}

void TrackingManager::setNatNetHostCommandPort(int hostCommandPort){
	mNatNetHostCommandPort = hostCommandPort;
}

void TrackingManager::setNatNetHostDataPort(int hostDataPort){
	mNatNetHostDataPort = hostDataPort;
}

void TrackingManager::setFrameEvaluationMethod(FRAME_EVALUATION_METHOD eval){
	if(mEvaluator != nullptr)
		mEvaluator->setEvaluationMethod(eval);
	mEval = eval;
}

void TrackingManager::addRigidBodyEventListener(RigidBodyEventListener* listener){
	mEvaluator->addRigidBodyEventListener(listener);
}

};