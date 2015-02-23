#include "ARLib\Sound\SoundSource.h"

#include "AL\al.h"
#include "AL\alc.h"

namespace ARLib{
	
SoundSource::SoundSource(){
	alGenBuffers((ALuint)1, &mSoundBuffer);
	alGenSources((ALuint)1, &mSource);

	alSourcef(mSource, AL_PITCH, 1);
	alSourcef(mSource, AL_GAIN, 1);
	alSourcei(mSource, AL_LOOPING, AL_FALSE);
}

SoundSource::~SoundSource(){
	alDeleteBuffers((ALuint)1, &mSoundBuffer);
	alDeleteSources((ALuint)1, &mSource);
}


const Sound *SoundSource::getSound() const{
	return mCurrentSound;
}

bool SoundSource::isPlaying() const{
	ALint playing;
	alGetSourcei(mSource, AL_SOURCE_STATE, &playing);
	if(playing == AL_PLAYING){
		return true;
	}
	return false;
}

void SoundSource::setGain(float gain){
	alSourcef(mSource, AL_GAIN, gain);
}

void SoundSource::setPitch(float pitch){
	alSourcef(mSource, AL_PITCH, pitch);
}

void SoundSource::setLoop(bool loop){
	alSourcei(mSource, AL_LOOPING, (ALint)loop);
}

void SoundSource::setPosition(const Ogre::Vector3& pos){
	alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
}

void SoundSource::setVelocity(const Ogre::Vector3& vel){
	alSource3f(mSource, AL_VELOCITY, vel.x, vel.y, vel.z);
}

void SoundSource::playSound(const char* soundPath){
	mCurrentSound = SoundManager::instance().getSound(soundPath);

	alDeleteBuffers(1, &mSoundBuffer); //dunno why but have to regenerate buffers on update :s
	alGenBuffers(1, &mSoundBuffer);
	alBufferData(mSoundBuffer, mCurrentSound->getFormat(), mCurrentSound->getData(), mCurrentSound->getSize(), mCurrentSound->getSampleRate());

	alSourcei(mSource, AL_BUFFER, mSoundBuffer);
	alSourcePlay(mSource);
}

};