#include "ARLib/Tracking/FrameEvaluator.h"
#include <math.h>

namespace ARLib{
	
FrameEvaluator::FrameEvaluator(unsigned int frameBufferSize)
    : mFrameBufferSize(frameBufferSize + 2)
    , mEval(FRAME_ROUND){
    QueryPerformanceFrequency(&mFreq);

    mRiftFrames = new TimedFrame[mFrameBufferSize];
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        mRiftFrames[i].mBody = new RigidBody();
        QueryPerformanceCounter(&mRiftFrames[i].mTimestamp);
    }
}

FrameEvaluator::~FrameEvaluator(){
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        delete mRiftFrames[i].mBody;
    }
    delete [] mRiftFrames;
}

RigidBody* FrameEvaluator::evaluateRift(const LARGE_INTEGER& retroActiveQueryTime){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    LARGE_INTEGER currentTime;
    currentTime.QuadPart = 0;

    unsigned int index = 1;
    for( ; currentTime.QuadPart < retroActiveQueryTime.QuadPart && index < mFrameBufferSize - 1; index++){
        currentTime.QuadPart += mRiftFrames[index-1].mTimestamp.QuadPart - mRiftFrames[index].mTimestamp.QuadPart;
    }
    double weight =  static_cast<double>(retroActiveQueryTime.QuadPart - mRiftFrames[index].mTimestamp.QuadPart) / static_cast<double>(mRiftFrames[index-1].mTimestamp.QuadPart - mRiftFrames[index].mTimestamp.QuadPart);

    RigidBody *lowerBody = mRiftFrames[index-1].mBody;
    RigidBody *higherBody = mRiftFrames[index].mBody;

    switch(mEval){
    case FRAME_ROUND:
        if(weight < 0.5){
            return lowerBody;
        }else{
            return higherBody;
        }
        break;
    case FRAME_FLOOR:
        return lowerBody;
        break;
    case FRAME_CEIL:
        return higherBody;
        break;
    case FRAME_INTERPOLATE_LINEAR:
        //todo
        return nullptr; //not implemented yet
        break;
    default:
        printf("Frame Evaluation Method unknown! Nothing will happen!");
        return nullptr;
    }
    return nullptr; //shouldn't get here;
}

void FrameEvaluator::evaluate(){
    for(unsigned int j = 0; j < mRigidBodies.size(); j++){
        for(unsigned int i = 0; i < mFrame->mNRigidBodys; i++){
            if(mFrame->mRbs[i]->mID == mRigidBodies[j]->getRigidBodyID()){
                mRigidBodies[j]->onChange(mFrame->mRbs[i]);
                break;
            }
        }
    }
}

void FrameEvaluator::updateFrame(RBFrame *frame){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    delete mRiftFrames[mFrameBufferSize-1].mBody;
    for(unsigned int i = mFrameBufferSize - 1; i > 0; i--){
        mRiftFrames[i] = mRiftFrames[i-1];
    }
    for(unsigned int i = 0; i < frame->mNRigidBodys; i++){
        if(frame->mRbs[i]->mID == RIFT_BODY_ID){
            mRiftFrames[0].mBody = new RigidBody(*frame->mRbs[i]);
            QueryPerformanceCounter(&mRiftFrames[0].mTimestamp);
        }
    }
    mFrame = frame;
}

void FrameEvaluator::addRigidBodyEventListener(RigidBodyEventListener* listener){
	mRigidBodies.push_back(listener);
}

void FrameEvaluator::setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod){
    mEval = evalMethod;
}

};