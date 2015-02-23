#ifndef ARLIB_SOUND_LISTENER_H
#define ARLIB_SOUND_LISTENER_H

#include "OGRE\Ogre.h"

namespace ARLib{

class SoundListener{
public:
	static SoundListener& instance(){
		static SoundListener sLInstance;
		return sLInstance;
	}
	~SoundListener();
	
	void attachToNode(Ogre::SceneNode* sceneNode);
	void update(float dt);
private:
	SoundListener();
	SoundListener(const SoundListener& sm);
	SoundListener& operator=(const SoundListener& sm);

	Ogre::SceneNode* mSceneNode;
};


}; //namespace ARLib

#endif //ARLIB_SOUND_LISTENER