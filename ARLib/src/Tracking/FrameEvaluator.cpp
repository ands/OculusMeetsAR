#include "ARLib/Tracking/FrameEvaluator.h"
#include <algorithm>
#include <math.h>

namespace ARLib{
	
FrameEvaluator::FrameEvaluator(unsigned int frameBufferSize)
    : mFrameBufferSize(frameBufferSize + 2)
    , mEval(FRAME_ROUND)
	, mCurrentFrame(nullptr){
}

FrameEvaluator::~FrameEvaluator(){
	for(std::map<unsigned int, TimedFrame*>::iterator it = mRigidBodyHistories.begin(); it != mRigidBodyHistories.end(); it++){
		delete (*it).second;
    }
	delete mCurrentFrame;
}

RigidBody* FrameEvaluator::evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
    long long currentTime = 0;
	
	if(mRigidBodyHistories.find(ID) == mRigidBodyHistories.end()){
		return nullptr;
	}
	TimedFrame *frames = mRigidBodyHistories[ID];

    unsigned int index = 1;
    for( ; currentTime < retroActiveQueryTime && index < mFrameBufferSize - 1; index++){
        currentTime += frames[index-1].mTimestamp - frames[index].mTimestamp;
    }
    double weight =  static_cast<double>(retroActiveQueryTime - frames[index].mTimestamp) / static_cast<double>(frames[index-1].mTimestamp - frames[index].mTimestamp);

    RigidBody *lowerBody = frames[index-1].mBody;
    RigidBody *higherBody = frames[index].mBody;

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

void FrameEvaluator::addRigidBodyEventListener(RigidBodyEventListener* listener){
	if(listener != nullptr){
		if(listener->isRiftListener() && mRigidBodyHistories.find(listener->getRigidBodyID()) == mRigidBodyHistories.end()){
			TimedFrame *t = new TimedFrame[mFrameBufferSize];
			LARGE_INTEGER current;
			QueryPerformanceCounter(&current);
			mRigidBodyHistories[listener->getRigidBodyID()] = t;
			for(unsigned int i = 0; i < mFrameBufferSize; i++){
				t[i].mTimestamp = current.QuadPart;
				t[i].mBody = nullptr;
			}
		}
		mRigidBodies.push_back(listener);
	}
}

void FrameEvaluator::setEvaluationMethod(FRAME_EVALUATION_METHOD evalMethod){
    mEval = evalMethod;
}


/**********************************************************************************************************
GenericNatNetEvaluator
**********************************************************************************************************/
GenericNatNetEvaluator::GenericNatNetEvaluator(unsigned int frameBufferSize)
	: FrameEvaluator(frameBufferSize){

}

/**********************************************************************************************************
NatNetRiftEvaluator
**********************************************************************************************************/
NatNetRiftEvaluator::NatNetRiftEvaluator(unsigned int frameBufferSize)
	: GenericNatNetEvaluator(frameBufferSize){

}

RigidBody* NatNetRiftEvaluator::evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime){
	return nullptr;
}

void NatNetRiftEvaluator::evaluate(){

}

void NatNetRiftEvaluator::updateFrame(RBFrame *frame){
    tthread::lock_guard<tthread::mutex> guard(mMutex);

	for(std::map<unsigned int, TimedFrame*>::iterator it = mRigidBodyHistories.begin(); it != mRigidBodyHistories.end(); ++it){
		TimedFrame *t = (*it).second;
		delete t[mFrameBufferSize-1].mBody;
		for(unsigned int i = mFrameBufferSize - 1; i > 0; i--){
			t[i] = t[i-1];
		}
		for(unsigned int i = 0 ; i < frame->mNRigidBodys; i++){
			if(frame->mRbs[i]->mID == (*it).first){
				t[0].mBody = new RigidBody(*frame->mRbs[i]);
				LARGE_INTEGER temp;
				QueryPerformanceCounter(&temp);
				t[0].mTimestamp = temp.QuadPart;
			}
		}
	}
	mCurrentFrame = frame;
}
/**********************************************************************************************************
NatNetEvaluator
**********************************************************************************************************/
NatNetEvaluator::NatNetEvaluator(unsigned int frameBufferSize)
	: GenericNatNetEvaluator(frameBufferSize){

}

RigidBody* NatNetEvaluator::evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime){
	tthread::lock_guard<tthread::mutex> guard(mMutex);
    long long currentTime = 0;
	
	if(mRigidBodyHistories.find(ID) == mRigidBodyHistories.end()){
		return nullptr;
	}
	TimedFrame *frames = mRigidBodyHistories[ID];

    unsigned int index = 1;
    for( ; currentTime < retroActiveQueryTime && index < mFrameBufferSize - 1; index++){
        currentTime += frames[index-1].mTimestamp - frames[index].mTimestamp;
    }
    double weight =  static_cast<double>(retroActiveQueryTime - frames[index].mTimestamp) / static_cast<double>(frames[index-1].mTimestamp - frames[index].mTimestamp);

    RigidBody *lowerBody = frames[index-1].mBody;
    RigidBody *higherBody = frames[index].mBody;

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
		if(higherBody != nullptr && lowerBody != nullptr){
			return interpolateRigidBodies(lowerBody, higherBody, static_cast<float>( min(1.0, max(0.0, weight))));
		}
        break;
    default:
        printf("Frame Evaluation Method unknown! Nothing will happen!");
        return nullptr;
    }
    return nullptr; //shouldn't get here;
}

void NatNetEvaluator::evaluate(){
	if(mCurrentFrame != nullptr){
		for(unsigned int i = 0; i < mRigidBodies.size(); i++){
			for(unsigned int j = 0; j < mCurrentFrame->mNRigidBodys; j++){
				if(mRigidBodies[i]->getRigidBodyID() == mCurrentFrame->mRbs[j]->mID){
					if(mRigidBodies[i]->isCalibrating()){
						mRigidBodies[i]->setReferenceOrientation(mCurrentFrame->mRbs[j]->mqX, mCurrentFrame->mRbs[j]->mqY, mCurrentFrame->mRbs[j]->mqZ, mCurrentFrame->mRbs[j]->mqW);
						mRigidBodies[i]->setReferencePosition(mCurrentFrame->mRbs[j]->mX, mCurrentFrame->mRbs[j]->mY, mCurrentFrame->mRbs[j]->mZ);
						mRigidBodies[i]->calibrate(false);
					}
					RigidBody rb = RigidBody(*mCurrentFrame->mRbs[j]);
					rb.mqW = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqX - mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqY - mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqZ;
					rb.mqX = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqX + mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqZ + mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqY;
					rb.mqY = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqY + mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqZ + mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqX;
					rb.mqZ = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqZ - mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqY + mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqX + mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqW;
					mRigidBodies[i]->onChange(&rb);
					break;
				}
			}
		}
	}
}

void NatNetEvaluator::updateFrame(RBFrame *frame){
    tthread::lock_guard<tthread::mutex> guard(mMutex);
	for(std::map<unsigned int, TimedFrame*>::iterator it = mRigidBodyHistories.begin(); it != mRigidBodyHistories.end(); ++it){
		TimedFrame *t = (*it).second;
		delete t[mFrameBufferSize-1].mBody;
		for(unsigned int i = mFrameBufferSize - 1; i > 0; i--){
			t[i] = t[i-1];
		}
		for(unsigned int i = 0 ; i < frame->mNRigidBodys; i++){
			if(frame->mRbs[i]->mID == (*it).first){
				t[0].mBody = new RigidBody(*frame->mRbs[i]);
				LARGE_INTEGER temp;
				QueryPerformanceCounter(&temp);
				t[0].mTimestamp = temp.QuadPart;
			}
		}
	}
	mCurrentFrame = frame;
}
/**********************************************************************************************************
RiftEvaluator
**********************************************************************************************************/
RiftEvaluator::RiftEvaluator(unsigned int frameBufferSize)
	: FrameEvaluator(frameBufferSize){

}

RigidBody* RiftEvaluator::evaluateRigidBody(unsigned int ID, const long long& retroActiveQueryTime){
	if(mRigidBodyHistories.find(ID) != mRigidBodyHistories.end()){
		TimedFrame *t = mRigidBodyHistories[ID];
		long long currentTime = 0;
		unsigned int index = 1;
		for( ; currentTime < retroActiveQueryTime && index < mFrameBufferSize - 1; index++){
			currentTime += t[index-1].mTimestamp - t[index].mTimestamp;
		}
		double weight =  static_cast<double>(retroActiveQueryTime - t[index].mTimestamp) / static_cast<double>(t[index-1].mTimestamp - t[index].mTimestamp);

		RigidBody *lowerBody = t[index-1].mBody;
		RigidBody *higherBody = t[index].mBody;

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
	return nullptr;
}

void RiftEvaluator::evaluate(){
	//Construct Frame from Rift Data
	RBFrame *tempFrame = new RBFrame(mRifts.size(), 0, 0, 0, 0, true);
	for(unsigned int i = 0; i < mRifts.size(); i++){
		float pos[3]; float ori[4];
		mRifts[i].first->getPose(pos, ori);
		RigidBody *tempBody = new RigidBody(0, pos[0], pos[1], pos[2], ori[0], ori[1], ori[2], ori[3] );
		tempBody->mID = mRifts[i].second;
		tempFrame->addRigidBody(i, tempBody);
	}
	delete mCurrentFrame;
	mCurrentFrame = tempFrame;

	//fish out history data
	for(std::map<unsigned int, TimedFrame*>::iterator it = mRigidBodyHistories.begin(); it != mRigidBodyHistories.end() ; ++it){
		for(unsigned int i = 0; i < mCurrentFrame->mNRigidBodys; i++){
			if((*it).first == mCurrentFrame->mRbs[i]->mID){
				TimedFrame* t = (*it).second;
				delete t->mBody;
				for(unsigned int j = mFrameBufferSize - 1; j > 0 ; j--){
					t[i] = t[i-1];
				}
				t[0].mBody = new RigidBody(*mCurrentFrame->mRbs[i]);
				LARGE_INTEGER tempStamp;
				QueryPerformanceCounter(&tempStamp);
				t[0].mTimestamp = tempStamp.QuadPart;
				break;
			}
		}
	}
	//Let all RigidBodyEventListeners know
	for(unsigned int i = 0; i < mRigidBodies.size(); i++){
		for(unsigned int j = 0; j < mCurrentFrame->mNRigidBodys; j++){
			if(mRigidBodies[i]->getRigidBodyID() == mCurrentFrame->mRbs[j]->mID){
				if(mRigidBodies[i]->isCalibrating()){
					mRigidBodies[i]->setReferenceOrientation(mCurrentFrame->mRbs[j]->mqX, mCurrentFrame->mRbs[j]->mqY, mCurrentFrame->mRbs[j]->mqZ, mCurrentFrame->mRbs[j]->mqW);
					mRigidBodies[i]->setReferencePosition(mCurrentFrame->mRbs[j]->mX, mCurrentFrame->mRbs[j]->mY, mCurrentFrame->mRbs[j]->mZ);
					mRigidBodies[i]->calibrate(false);
				}
				RigidBody rb = RigidBody(*mCurrentFrame->mRbs[j]);
				rb.mqW = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqX - mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqY - mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqZ;
				rb.mqX = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqX + mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqZ + mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqY;
				rb.mqY = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqY + mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqZ + mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqW - mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqX;
				rb.mqZ = mRigidBodies[i]->mRefQW * mCurrentFrame->mRbs[j]->mqZ - mRigidBodies[i]->mRefQX * mCurrentFrame->mRbs[j]->mqY + mRigidBodies[i]->mRefQY * mCurrentFrame->mRbs[j]->mqX + mRigidBodies[i]->mRefQZ * mCurrentFrame->mRbs[j]->mqW;
				mRigidBodies[i]->onChange(&rb);
				break;
			}
		}
	}
}

};