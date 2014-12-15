#ifndef RIFTSCENENODE_H
#define RIFTSCENENODE_H

#include "OGRE/Ogre.h"
#include "ARLib/Oculus/Rift.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

namespace ARLib {

class RiftSceneNode : public RigidBodyEventListener
{
	public:
		RiftSceneNode(Rift *rift, Ogre::SceneManager *sceneManager, float zNear, float zFar, unsigned int rigidBodyID);
		~RiftSceneNode();

		Rift *getRift() { return rift; }
		Ogre::SceneNode *getNode() { return bodyNode; }
		Ogre::Camera *getLeftCamera() { return cameras[0]; }
		Ogre::Camera *getRightCamera() { return cameras[1]; }

		void setPitch(Ogre::Radian angle);
		void setYaw(Ogre::Radian angle);

		virtual void onChange(RigidBody *rb);

	private:
		Rift *rift;
		Ogre::Camera *cameras[2];
		Ogre::SceneNode *headNode;
		Ogre::SceneNode *bodyNode;
		Ogre::SceneNode *bodyTiltNode;
};

}; // ARLib namespace

#endif
