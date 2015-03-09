#include "SimpleSoundController.h"
#include <time.h>

SimpleSoundController::SimpleSoundController(float rate, const std::string& mainSound, const std::string& secSound)
	: mPart(0.0f)
	, mMainSound(mainSound)
	, mInc(true)
	, mRate(rate)
	, mAlreadyPlayed(0.0f){
	mSecSounds.push_back(secSound);
	mMainSource.playSound(mainSound.c_str());
	mMainSource.setLoop(true);
	
	mSecSource.playSound(secSound.c_str());
	mCurrentDuration = mSecSource.getSound()->getDuration();
}

void SimpleSoundController::updatePositions(const Ogre::Vector3& pos){
	mMainSource.setPosition(pos);
	mSecSource.setPosition(pos);
}

void SimpleSoundController::addSecSound(const std::string& soundFile){
	mSecSounds.push_back(soundFile);
}

bool SimpleSoundController::frameRenderingQueued(const Ogre::FrameEvent& evt){
	mAlreadyPlayed += evt.timeSinceLastFrame;
	if(mAlreadyPlayed < (0.5f/mRate)){
		mPart += mRate * evt.timeSinceLastFrame;
	}
	else if(mAlreadyPlayed > mCurrentDuration - (0.5f/mRate)){
		mPart -= mRate * evt.timeSinceLastFrame;
	}
	
	if(!mSecSource.isPlaying()){
		mPart = 0.0f;
		mAlreadyPlayed = 0.0f;
		int random = rand() % mSecSounds.size();
		mSecSource.playSound(mSecSounds[random].c_str());
	}
	mMainSource.setGain(1.0f - mPart);
	mSecSource.setGain(mPart);
	printf("%f\n",mPart);
	return true;
}
