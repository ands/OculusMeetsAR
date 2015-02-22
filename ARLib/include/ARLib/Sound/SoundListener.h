#ifndef ARLIB_SOUND_LISTENER_H
#define ARLIB_SOUND_LISTENER_H

#include "OGRE\Ogre.h"

namespace ARLib{

class SoundListener : public Ogre::FrameListener{
public:
	static SoundListener& instance(){
		static SoundListener sLInstance;
		return sLInstance;
	}
	~SoundListener();
	
	void attachToNode(Ogre::SceneNode* sceneNode);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
private:
	SoundListener();
	SoundListener(const SoundListener& sm);
	SoundListener& operator=(const SoundListener& sm);

	Ogre::SceneNode* mSceneNode;
};


}; //namespace ARLib

#endif //ARLIB_SOUND_LISTENER