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
	private:
		ALenum mFormat;
		void *mData;
		unsigned int mSize;
		unsigned int mSampleRate;
	};

};

#endif