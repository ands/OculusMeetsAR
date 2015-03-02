#include "ARLib/Tracking/RigidBodyFrame.h"
#include <assert.h>

namespace ARLib{	
	_RigidBody::_RigidBody(int id, float x, float y, float z, float qx, float qy, float qz, float qw)
		: mX(x)
		, mY(y)
		, mZ(z)
		, mqX(qx)
		, mqY(qy)
		, mqZ(qz)
		, mqW(qw)
		, mID(id){

	}

	_RBFrame::_RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool ownership)
		: mNRigidBodies(nRigidBodys)
		, mFrameID(frameID) 
		, mTimestamp(timestamp)
		, mLatency(latency)
        , mValid(valid)
        , mOwnership(ownership){
		mRbs = new RigidBody*[mNRigidBodies];
	}

	_RBFrame::~_RBFrame(){
        if(mOwnership){
            for(unsigned int i = 0; i < mNRigidBodies ; i++){
                delete mRbs[i];
            }
        }
        delete [] mRbs;
	}

	RigidBody*& _RBFrame::operator[](unsigned int index){
		assert(index >= mNRigidBodies);
		return mRbs[index];
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