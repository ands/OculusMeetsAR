#ifndef ARLIB_SOUND_SOURCE_H
#define ARLIB_SOUND_SOURCE_H

#include "OGRE\Ogre.h"
#include "ARLib\Sound\SoundManager.h"

typedef unsigned int ALuint;

namespace ARLib{

class SoundSource{
public:
	SoundSource();
	~SoundSource();

	const Sound *getSound() const;
	bool isPlaying() const;
	void setGain(float gain);
	void setPitch(float pitch);
	void setLoop(bool loop);
	void setPosition(const Ogre::Vector3& pos);
	void setVelocity(const Ogre::Vector3& vel);
	void playSound(const char* soundPath);
private:
	Sound *mCurrentSound;
	ALuint mSoundBuffer;
	ALuint mSource;
};

}; //namespace ARLib


#endif //ARLIB_SOUND_SOURCE