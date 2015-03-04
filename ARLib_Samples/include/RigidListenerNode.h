#ifndef RIGID_LISTENER_NODE_H
#define RIGID_LISTENER_NODE_H

#include "ARLib/Tracking/RigidBodyEventListener.h"

namespace Ogre
{
	class SceneManager;
	class SceneNode;
}

class RigidListenerNode : public ARLib::RigidBodyEventListener
{
public:
	RigidListenerNode(Ogre::SceneNode *parent, Ogre::SceneManager *sceneManager, unsigned int rigidBodyID);
	void onChange(const ARLib::RigidBody* rb);
    Ogre::SceneNode *getSceneNode();
private:
	Ogre::SceneNode *mRigidBodyNode;
	Ogre::SceneNode *mRigidBodyOrientationNode;
	Ogre::SceneNode *mRigidBodyCalibNode;
};


#endif