#include "ARLib/Sound/SoundManager.h"

namespace ARLib{

SoundManager::~SoundManager(){
	//clean up mSounds
}

Sound* SoundManager::getSound(const char* soundFilename){
	bool successfullyCreated = true;
	if(mSounds.find(soundFilename) == mSounds.end()){
		successfullyCreated = loadSound(soundFilename);
	}
	if(!successfullyCreated){
		return nullptr;
	}
	return mSounds.at(soundFilename);
}

bool SoundManager::loadSound(const char* filename){
	FILE* fp = nullptr;
	errno_t err = fopen_s(&fp,filename, "r");
	if(err != 0){
		printf("Could Not Open %s\n", filename);
		fclose(fp);
		return false;
	}

	char ChunkID[4];
	fread(ChunkID, 4, sizeof(char), fp);

	if(!strcmp(ChunkID, "RIFF")){
		printf("ChunkID: %s != RIFF\n", ChunkID);
		fclose(fp);
		return false;
	}

	fseek(fp, 8, SEEK_SET);
	char format[4];
	fread(format, 4, sizeof(char), fp);
	if(!strcmp(format, "WAVE")){
		printf("Wrong Format %s\n", format);
		fclose(fp);
		return false;
	}

	char subChunk1ID[4];
	fread(subChunk1ID, 4, sizeof(char), fp);

	if(!strcmp(subChunk1ID, "fmt ")){
		printf("Possible File corruption\n");
		fclose(fp);
		return false;
	}

	unsigned int subChunk1Size;
	fread(&subChunk1Size, 1, sizeof(unsigned int), fp);

	unsigned int subChunk2Location = (unsigned int)ftell(fp) + subChunk1Size;

	unsigned short audioFormat;
	fread(&audioFormat, 1, sizeof(unsigned short), fp);
	if(audioFormat != 1){
		printf("Audio Is Not PCM \n");
		fclose(fp);
		return false;
	}

	unsigned short numChannels;
	fread(&numChannels, 1, sizeof(unsigned short), fp);
	unsigned int sampleRate;
	fread(&sampleRate, 1, sizeof(unsigned int), fp);
	
	fseek(fp, 34, SEEK_SET);

	unsigned short bitsPerSample;
	fread(&bitsPerSample, 1, sizeof(unsigned short), fp);

	int ALFormat;
	if(numChannels == 1 && bitsPerSample == 8){
		ALFormat = AL_FORMAT_MONO8;
	}else if(numChannels == 1 && bitsPerSample == 16){
		ALFormat = AL_FORMAT_MONO16;
	}else if(numChannels == 2 && bitsPerSample == 8){
		ALFormat = AL_FORMAT_STEREO16;
	}else if(numChannels == 2 && bitsPerSample == 16){
		ALFormat = AL_FORMAT_STEREO16;
	}else{
		printf("Audio Format not supported by OpenAL\n");
		fclose(fp);
		return false;
	}

	fseek(fp, subChunk2Location, SEEK_SET);
	char subChunk2ID[4];
	fread(subChunk2ID, 4, sizeof(char), fp);
	if(!strcmp(subChunk2ID, "data")){
		printf("Possible File corruption\n");
		fclose(fp);
		return false;
	}
	unsigned int subChunk2Size;
	fread(&subChunk2Size, 1, sizeof(unsigned int), fp);
	void *data = new unsigned char[subChunk2Size];
	fread(data, subChunk2Size, sizeof(unsigned char), fp);
	fclose(fp);

	Sound* newSound = new Sound(ALFormat, data, subChunk2Size, sampleRate);

	mSounds.insert(std::pair<const char*, Sound*>(filename, newSound));
	return true;
}

};