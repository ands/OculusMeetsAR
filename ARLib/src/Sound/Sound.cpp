#include "ARLib/Sound/Sound.h"

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


};