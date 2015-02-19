#include "ARLib/Tracking/RigidBodyFrame.h"

namespace ARLib{

	_RigidBody::_RigidBody(unsigned int nMarker)
		: mNMarker(nMarker){
		mMarkers = new Marker[mNMarker];
	}

	_RigidBody::~_RigidBody(){
		delete [] mMarkers;
	}

	void _RigidBody::addMarker(unsigned int index, int markerID, float markerSize, float x, float y, float z){
		mMarkers[index].mMarkerID = markerID;
		mMarkers[index].mMarkerSize = markerSize;
		mMarkers[index].mX = x;
		mMarkers[index].mY = y;
		mMarkers[index].mZ = z;
	};

	_RBFrame::_RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool ownership)
		: mNRigidBodys(nRigidBodys)
		, mFrameID(frameID) 
		, mTimestamp(timestamp)
		, mLatency(latency)
		, mChange(true)
        , mValid(valid)
        , mOwnership(ownership){
			mRbs = new RigidBody*[mNRigidBodys];
	}

	_RBFrame::~_RBFrame(){
        if(mOwnership){
            for(unsigned int i = 0; i < mNRigidBodys ; i++){
                delete mRbs[i];
            }
            delete [] mRbs;
        }
	}

	void _RBFrame::addRigidBody(unsigned int index, RigidBody* rb){
		//test index out of range
		mRbs[index] = rb;
	}
    RigidBody *interpolateRigidBodies(RigidBody *lRigidBody, RigidBody *rRigidBody, float weight){
    RigidBody *newRigidBody = new RigidBody();
    newRigidBody->mX = weight * lRigidBody->mX + (1.0f - weight) * rRigidBody->mX;
    newRigidBody->mY = weight * lRigidBody->mY + (1.0f - weight) * rRigidBody->mY;
    newRigidBody->mZ = weight * lRigidBody->mZ + (1.0f - weight) * rRigidBody->mZ;

    float omega = lRigidBody->mqX * rRigidBody->mqX + lRigidBody->mqY * rRigidBody->mqY + lRigidBody->mqZ * rRigidBody->mqZ + lRigidBody->mqW * rRigidBody->mqW;
    if(omega >= 0.95f){
        newRigidBody->mqX = weight * lRigidBody->mqX + (1.0f - weight) * rRigidBody->mqX;
        newRigidBody->mqY = weight * lRigidBody->mqY + (1.0f - weight) * rRigidBody->mqY;
        newRigidBody->mqZ = weight * lRigidBody->mqZ + (1.0f - weight) * rRigidBody->mqZ;
        newRigidBody->mqW = weight * lRigidBody->mqW + (1.0f - weight) * rRigidBody->mqW;
    }else{
        omega = acosf(omega);
        float sinOmega = sinf(omega);
        float leftWeight = sinf((1.0f - weight)*omega)/sinOmega;
        float rightWeight = sinf(weight*omega)/sinOmega;
        newRigidBody->mqX = leftWeight * lRigidBody->mqX + rightWeight * rRigidBody->mqX;
        newRigidBody->mqY = leftWeight * lRigidBody->mqY + rightWeight * rRigidBody->mqY;
        newRigidBody->mqZ = leftWeight * lRigidBody->mqZ + rightWeight * rRigidBody->mqZ;
        newRigidBody->mqW = leftWeight * lRigidBody->mqW + rightWeight * rRigidBody->mqW;
    }
    return newRigidBody;
}

};