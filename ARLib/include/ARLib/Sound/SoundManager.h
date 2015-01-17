#ifndef ARLIB_SOUND_MANAGER_H
#define ARLIB_SOUND_MANAGER_H

#include <map>
#include "ARLib/Sound/Sound.h"


namespace ARLib{

	class SoundManager{
	public:
		static SoundManager& instance(){
			static SoundManager sMInstance;
			return sMInstance;
		}
		~SoundManager();

		Sound* getSound(const char *soundFilename);
	private:
		SoundManager(){};
		SoundManager(const SoundManager& sm);
		SoundManager& operator=(const SoundManager& sm);


		bool loadSound(const char* filename);
		std::map<const char*, Sound*> mSounds;
	};

};


#endif