#ifndef ARLIB_SOUND_H
#define ARLIB_SOUND_H

#include "AL\al.h"
#include "AL\alc.h"

namespace ARLib{

	class Sound{
	public:
		Sound(ALenum format, void *data, unsigned int size, unsigned int sampleRate);
		~Sound();

		ALenum getFormat() const{
			return mFormat;
		}
		const void* getData() const{
			return mData;
		}
		unsigned int getSize() const{
			return mSize;
		}
		unsigned int getSampleRate() const{
			return mSampleRate;
		}
		
		float getDuration()const{
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
	private:
		ALenum mFormat;
		void *mData;
		unsigned int mSize;
		unsigned int mSampleRate;
	};

};

#endif