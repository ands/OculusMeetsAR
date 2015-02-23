#include "ARLib/Tracking/RigidBodyEventListener.h"

namespace ARLib{

RigidBodyEventListener::RigidBodyEventListener(unsigned int rigidBodyID, bool isRiftListener)
		: mRigidBodyID(rigidBodyID)
		, mRiftListener(isRiftListener)
		, mRefX(0.0f)
		, mRefY(0.0f)
		, mRefZ(0.0f)
		, mRefQX(0.0f)
		, mRefQY(0.0f)
		, mRefQZ(0.0f)
		, mRefQW(1.0f){

}

unsigned int RigidBodyEventListener::getRigidBodyID()const{
	return mRigidBodyID;
}
        
bool RigidBodyEventListener::isRiftListener()const{
	return mRiftListener;
}

bool RigidBodyEventListener::isCalibrating()const{
	return mCalibrating;
}

void RigidBodyEventListener::calibrate(bool t){
	mCalibrating = t;
}

void RigidBodyEventListener::setReferencePosition(float x, float y, float z){
	mRefX = x;
	mRefY = y;
	mRefZ = z;
}

void RigidBodyEventListener::setReferenceOrientation(float qx, float qy, float qz, float qw){
	mRefQX = qx;
	mRefQY = qy;
	mRefQZ = qz;
	mRefQW = qw;
}

RiftRigidBodyEventListener::RiftRigidBodyEventListener(Rift *rift, unsigned int rigidBodyID)
	: RigidBodyEventListener(rigidBodyID, true)
	, mRift(rift){

}

Rift* RiftRigidBodyEventListener::getRift(){
	return mRift;
}

};