#include "ARLib/Tracking/FrameEvaluator.h"
#include <algorithm>
#include <math.h>

namespace ARLib{
	
FrameEvaluator::FrameEvaluator(unsigned int frameBufferSize)
    : mFrameBufferSize(frameBufferSize + 2)
    , mEval(FRAME_ROUND){
    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);
    mFreq = temp.QuadPart;

    mRiftFrames = new TimedFrame[mFrameBufferSize];
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        mRiftFrames[i].mBody = new RigidBody();
        QueryPerformanceCounter(&temp);
        mRiftFrames[i].mTimestamp = temp.QuadPart;
    }
}

FrameEvaluator::~FrameEvaluator(){
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        delete mRiftFrames[i].mBody;
    }
    delete [] mRiftFrames;
}

RigidBody* FrameEvaluator::evaluateRift(const long long& retroActiveQueryTime){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    long long currentTime = 0;

    unsigned int index = 1;
    for( ; currentTime < retroActiveQueryTime && index < mFrameBufferSize - 1; index++){
        currentTime += mRiftFrames[index-1].mTimestamp - mRiftFrames[index].mTimestamp;
    }
    double weight =  static_cast<double>(retroActiveQueryTime - mRiftFrames[index].mTimestamp) / static_cast<double>(mRiftFrames[index-1].mTimestamp - mRiftFrames[index].mTimestamp);

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
        return interpolateRigidBodies(lowerBody, higherBody, static_cast<float>( min(1.0, max(0.0, weight))));
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
            LARGE_INTEGER temp;
            QueryPerformanceCounter(&temp);
            mRiftFrames[0].mTimestamp = temp.QuadPart;
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