#ifndef SIMPLE_SOUND_CONTROLLER_H
#define SIMPLE_SOUND_CONTROLLER_H

#include "ARLib\Sound\SoundSource.h"
#include <string>
#include <vector>

class SimpleSoundController : public Ogre::FrameListener{
public:
	SimpleSoundController(float rate, const std::string& mainSound, const std::string& secSound);

	void updatePositions(const Ogre::Vector3& pos);
	void addSecSound(const std::string& soundFile);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
private:
	std::string mMainSound;
	std::vector<std::string> mSecSounds;

	float mAlreadyPlayed;
	float mCurrentDuration;
	float mRate;
	bool mInc;
	float mPart;
	ARLib::SoundSource mMainSource;
	ARLib::SoundSource mSecSource;
};

#endif //SIMPLE_SOUND_CONTROLLER_H
