#include "ARLib/Tracking/FrameEvaluator.h"
#include <math.h>

namespace ARLib{
	
FrameEvaluator::FrameEvaluator(unsigned int frameBufferSize)
    : mFrameBufferSize(frameBufferSize + 2)
    , mEval(FRAME_ROUND){
    QueryPerformanceCounter(&mStarttime);
    QueryPerformanceFrequency(&mFreq);

    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    double time = static_cast<double>(((end.QuadPart - mStarttime.QuadPart)*1000)/mFreq.QuadPart);
    mFrames = new RBFrame*[mFrameBufferSize];
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        mFrames[i] = new RBFrame(0,0,0,0,false, true);
        mFrames[i]->mTimestamp = time;
    }
}

FrameEvaluator::~FrameEvaluator(){
    for(unsigned int i = 0; i < mFrameBufferSize ; i++){
        delete mFrames[i];
    }
    delete [] mFrames;
}

void FrameEvaluator::evaluate(float retroActiveQueryTime){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    double currentTime = 0.0;
    unsigned int index = 1;
	for( ; currentTime < retroActiveQueryTime && index < mFrameBufferSize - 1; index++){
        currentTime += mFrames[index-1]->mTimestamp - mFrames[index]->mTimestamp;
    }
    float weight = 0.0f;// (currentTime - retroActiveQueryTime) / mTime[index-1];

    RBFrame *lowerFrame = mFrames[index-1];
    RBFrame *higherFrame = mFrames[index];

    if(lowerFrame != nullptr && higherFrame != nullptr && lowerFrame->mValid & higherFrame->mValid){
        RigidBody *lowerRigidBody = nullptr;
        RigidBody *higherRigidBody = nullptr;
        for(unsigned int j = 0 ; j < mRigidBodies.size() ; j++){
            for(unsigned int i = 0; i < lowerFrame->mNRigidBodys; i++){
                if(mRigidBodies[j]->getRigidBodyID() == lowerFrame->mRbs[i]->mID){
                    lowerRigidBody = lowerFrame->mRbs[i];
                    break;
                }
            }
            if(mEval & 0x01){
                for(unsigned int i = 0; i < higherFrame->mNRigidBodys; i++){
                    if(mRigidBodies[j]->getRigidBodyID() == higherFrame->mRbs[i]->mID){
                        higherRigidBody = higherFrame->mRbs[i];
                    }
                }
            }
            switch(mEval){
            case FRAME_ROUND:
                if(weight < 0.5f && lowerRigidBody != nullptr){
                    mRigidBodies[j]->onChange(lowerRigidBody);
                }else if(weight >= 0.5f && higherRigidBody != nullptr){
                    mRigidBodies[j]->onChange(higherRigidBody);
                }
#ifdef ARLIB_DEBUG_OUTPUT
                else{
                    printf("Frames invalid! Nothing Happened");
                }
#endif
                break;
            case FRAME_FLOOR:
                if(lowerRigidBody != nullptr){
                    mRigidBodies[j]->onChange(lowerRigidBody);
                }
#ifdef ARLIB_DEBUG_OUTPUT
                else{
                    printf("Frames invalid! Nothing Happened");
                }
#endif
                break;
            case FRAME_CEIL:
                if(higherRigidBody != nullptr){
                    mRigidBodies[j]->onChange(higherRigidBody);
                }
#ifdef ARLIB_DEBUG_OUTPUT
                else{
                    printf("Frames invalid! Nothing Happened");
                }
#endif
                break;
            case FRAME_INTERPOLATE_LINEAR:
                if(lowerRigidBody != nullptr && higherRigidBody != nullptr){
                    //todo
                }
#ifdef ARLIB_DEBUG_OUTPUT
                else{
                    printf("Frames invalid! Nothing Happened");
                }
#endif
                break;
            default:
                printf("Frame Evaluation Method unknown! Nothing will happen!");
                return;
            }
        }
    }
#ifdef __DEBUG
    else{
        printf("Frames invalid! Nothing Happened");
    }
#endif
}

void FrameEvaluator::updateFrame(RBFrame *frame){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    delete mFrames[mFrameBufferSize-1];
    for(unsigned int i = mFrameBufferSize - 1; i > 0; i--){
        mFrames[i] = mFrames[i-1];
    }
    mFrames[0] = frame; 
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    mFrames[0]->mTimestamp += static_cast<double>(((end.QuadPart - mStarttime.QuadPart)*1000)/mFreq.QuadPart);
    printf("%f\n", mFrames[0]->mTimestamp);
}

void FrameEvaluator::addRigidBodyEventListener(RigidBodyEventListener* listener){
	mRigidBodies.push_back(listener);
}

void FrameEvaluator::setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod){
    mEval = evalMethod;
}

};