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

void SoundListener::update(float dt){
	Ogre::Vector3 globalPos = mSceneNode->_getDerivedPosition();
	Ogre::Quaternion globalOrientation = mSceneNode->_getDerivedOrientation();
	alListener3f(AL_POSITION, globalPos.x, globalPos.y, globalPos.z);
	float ori[6];
	Ogre::Vector3 at = globalOrientation * -Ogre::Vector3::UNIT_Z;
	Ogre::Vector3 up = globalOrientation * Ogre::Vector3::UNIT_Y;
	ori[0] = at.x;
	ori[1] = at.y;
	ori[2] = at.z;
	ori[3] = up.x;
	ori[4] = up.y;
	ori[5] = up.z;
	alListenerfv(AL_ORIENTATION, ori);
}

}; //namespace ARLib