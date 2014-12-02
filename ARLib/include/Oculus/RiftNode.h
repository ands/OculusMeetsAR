#ifndef RIFTNODE_H
#define RIFTNODE_H

#include "OGRE/Ogre.h"

namespace ARLib {

class RiftNode
{
	public:
		RiftNode(Ogre::SceneManager* sceneManager);
		~RiftNode();

		Ogre::SceneNode* getNode() { return mBodyNode; }
		Ogre::Camera* getLeftCamera() { return mCamLeft; }
		Ogre::Camera* getRightCamera() { return mCamRight; }

		void setIPD(float ipd);
		void setPose(Ogre::Quaternion orientation, Ogre::Vector3 position);
		void setPitch(Ogre::Radian angle);
		void setYaw(Ogre::Radian angle);

	private:
		Ogre::Camera* mCamLeft;
		Ogre::Camera* mCamRight;

		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;
};

}; // ARLib namespace

#endif
