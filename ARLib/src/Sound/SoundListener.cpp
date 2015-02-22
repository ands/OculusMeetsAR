#include "ARLib/Sound/SoundListener.h"
#include "AL/al.h"
#include "AL/alc.h"

namespace ARLib{
	
SoundListener::SoundListener()
	: mSceneNode(nullptr){
	float orientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0,0,0);
	alListenerfv(AL_ORIENTATION, orientation);
}

SoundListener::~SoundListener(){}

void SoundListener::attachToNode(Ogre::SceneNode* SceneNode){
	mSceneNode = SceneNode;	
}

bool SoundListener::frameRenderingQueued(const Ogre::FrameEvent& evt){
	Ogre::Vector3 globalPos = mSceneNode->_getDerivedPosition();
	alListener3f(AL_POSITION, globalPos.x, globalPos.y, globalPos.z);
	//alListenerfv(AL_ORIENATION, someArray);
	return true;
}

}; //namespace ARLib