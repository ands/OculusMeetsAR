#include "Tracking\FrameEvaluator.h"

namespace ARLib{
	
FrameEvaluator::~FrameEvaluator(){
	delete mLastFrame;
	delete mCurrentFrame;
}

void FrameEvaluator::evaluate(){
	
}

void FrameEvaluator::updateFrame(RBFrame *frame){
	delete mLastFrame;
	mLastFrame = mCurrentFrame;
	mCurrentFrame = frame;
}

};