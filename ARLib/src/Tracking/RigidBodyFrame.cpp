#include "Tracking\RigidBodyFrame.h"

namespace ARLib{

	void RigidBody::addMarker(unsigned int index, int markerID, float markerSize, Ogre::Vector3& markerPosition){
		mMarkers[index].mMarkerID = markerID;
		mMarkers[index].mMarkerSize = markerSize;
		mMarkers[index].mMarkerPosition = markerPosition;
	};

	RBFrame::RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency)
		: mNRigidBodys(nRigidBodys)
		, mFrameID(frameID) 
		, mTimestamp(timestamp)
		, mLatency(latency)
		, mChange(true){
			mRbs = new RigidBody*[mNRigidBodys];
	}

	RBFrame::~RBFrame(){
		for(unsigned int i = 0; i < mNRigidBodys ; i++){
			delete mRbs[i];
		}
		delete [] mRbs;
	}

	void RBFrame::addRigidBody(unsigned int index, RigidBody* rb){
		//test index out of range
		mRbs[index] = rb;
	}

};