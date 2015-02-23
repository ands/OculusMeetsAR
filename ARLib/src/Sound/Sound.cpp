#include "ARLib/Sound/Sound.h"

#include "AL\al.h"
#include "AL\alc.h"

namespace ARLib{

Sound::Sound(ALenum format, void *data, unsigned int size, unsigned int sampleRate)
	: mFormat(format)
	, mData(data)
	, mSize(size)
	, mSampleRate(sampleRate){

}

Sound::~Sound(){
	delete[] mData;
}

float Sound::getDuration()const{
	switch(mFormat){
	case AL_FORMAT_MONO8: 
		return static_cast<float>(mSize)/static_cast<float>(mSampleRate);
	case AL_FORMAT_MONO16: 
		return static_cast<float>(mSize)/static_cast<float>(mSampleRate*2);
	case AL_FORMAT_STEREO8: 
		return static_cast<float>(mSize)/static_cast<float>(mSampleRate*2);
	case AL_FORMAT_STEREO16: 
		return static_cast<float>(mSize)/static_cast<float>(mSampleRate*4);
	default:
		return -1.0f;
	}
}

};