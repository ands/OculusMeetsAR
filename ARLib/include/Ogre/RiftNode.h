#ifndef RIFTNODE_H
#define RIFTNODE_H

#include "OGRE/Ogre.h"
#include "Oculus/Rift.h"
#include "Tracking/RigidBodyEventListener.h"

namespace ARLib {

class RiftNode : public RigidBodyEventListener
{
	public:
		RiftNode(
			Rift *rift, Ogre::Root *root, Ogre::RenderWindow *renderWindow,
			Ogre::SceneManager *sceneManager, float zNear, float zFar,
			unsigned int rigidBodyID);
		~RiftNode();

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

		Ogre::Root *root;
		Ogre::SceneManager *riftSceneManager;
};

}; // ARLib namespace

#endif
