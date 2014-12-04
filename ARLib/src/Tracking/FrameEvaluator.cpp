#include "ARLib/Tracking/FrameEvaluator.h"

namespace ARLib{
	
FrameEvaluator::FrameEvaluator(){
	mLastFrame = new RBFrame(0,0,0,0);
	mCurrentFrame = new RBFrame(0,0,0,0);
}

FrameEvaluator::~FrameEvaluator(){
	if(mLastFrame != nullptr){
		delete mLastFrame;
	}
	if(mCurrentFrame != nullptr){
		delete mCurrentFrame;
	}
}

void FrameEvaluator::evaluate(){
	for(int i = 0; i < mCurrentFrame->mNRigidBodys ; i++){
		for(int j = 0; j < mRigidBodies.size() ; j++){
			if(mCurrentFrame->mRbs[i]->mID == mRigidBodies[j]->getRigidBodyID()){
				mRigidBodies[j]->onChange(mCurrentFrame->mRbs[i]);
			}
		}
	}
}

void FrameEvaluator::updateFrame(RBFrame *frame){
	delete mLastFrame;
	mLastFrame = mCurrentFrame;
	mCurrentFrame = frame;
}

void FrameEvaluator::registerRigidBodyEventListener(RigidBodyEventListener* listener){
	mRigidBodies.push_back(listener);
}

};