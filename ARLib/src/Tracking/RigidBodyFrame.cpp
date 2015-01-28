#include "ARLib/Tracking/RigidBodyFrame.h"

namespace ARLib{

	RigidBody::RigidBody(unsigned int nMarker)
		: mNMarker(nMarker){
		mMarkers = new Marker[mNMarker];
	}

	RigidBody::~RigidBody(){
		delete [] mMarkers;
	}

	void RigidBody::addMarker(unsigned int index, int markerID, float markerSize, float x, float y, float z){
		mMarkers[index].mMarkerID = markerID;
		mMarkers[index].mMarkerSize = markerSize;
		mMarkers[index].mX = x;
		mMarkers[index].mY = y;
		mMarkers[index].mZ = z;
	};

	RBFrame::RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool ownership)
		: mNRigidBodys(nRigidBodys)
		, mFrameID(frameID) 
		, mTimestamp(timestamp)
		, mLatency(latency)
		, mChange(true)
        , mValid(valid)
        , mOwnership(ownership){
			mRbs = new RigidBody*[mNRigidBodys];
	}
    
    RBFrame::RBFrame(const RBFrame& lFrame, const RBFrame& rFrame, float weight)
        : mOwnership(true){
        //todo
    }

	RBFrame::~RBFrame(){
        if(mOwnership){
            for(unsigned int i = 0; i < mNRigidBodys ; i++){
                delete mRbs[i];
            }
            delete [] mRbs;
        }
	}

	void RBFrame::addRigidBody(unsigned int index, RigidBody* rb){
		//test index out of range
		mRbs[index] = rb;
	}

};