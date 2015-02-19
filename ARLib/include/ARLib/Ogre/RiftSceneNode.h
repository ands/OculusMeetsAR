#ifndef RIFTSCENENODE_H
#define RIFTSCENENODE_H

#include <vector>
#include "OGRE/Ogre.h"
#include "ARLib/Oculus/Rift.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

namespace ARLib {

// eye visibility masks
const unsigned int VISIBILITY_FLAG_LEFT  (1 << 0);
const unsigned int VISIBILITY_FLAG_RIGHT (1 << 1);

class RenderTarget;

class RiftSceneNode : public RigidBodyEventListener
{
	public:
		RiftSceneNode(Rift *rift, Ogre::SceneManager *sceneManager, float zNear, float zFar, unsigned int rigidBodyID);
		~RiftSceneNode();

		Rift *getRift() { return rift; }
		Ogre::SceneNode *getBodyNode() { return bodyNode; }
		Ogre::SceneNode *getHeadNode() { return headNode; }
		Ogre::Camera *getLeftCamera() { return cameras[0]; }
		Ogre::Camera *getRightCamera() { return cameras[1]; }

		void setPitch(Ogre::Radian angle);
		void setYaw(Ogre::Radian angle);

		void addRenderTarget(RenderTarget *renderTarget);
		void removeRenderTarget(RenderTarget *renderTarget);
		void removeAllRenderTargets();

		virtual void onChange(const RigidBody *rb);

	private:
		Rift *rift;
		Ogre::Camera *cameras[2];
		Ogre::SceneNode *headNode;
		Ogre::SceneNode *bodyNode;
		Ogre::SceneNode *bodyTiltNode;
		std::vector<RenderTarget*> renderTargets;
};

}; // ARLib namespace

#endif
