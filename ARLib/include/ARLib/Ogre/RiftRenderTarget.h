#ifndef RIFTRENDERTARGET_H
#define RIFTRENDERTARGET_H

#include "OGRE/Ogre.h"
#include "ARLib/Ogre/RenderTarget.h"

namespace ARLib {

class Rift;
class RiftSceneNode;

class RiftRenderTarget : public RenderTarget
{
	public:
		RiftRenderTarget(Rift *rift, Ogre::Root *root, Ogre::RenderWindow *renderWindow);
		~RiftRenderTarget();

		Rift *getRift() { return rift; }

		// use one of the following to set the cameras for the render target:
		virtual void SetCameras(Ogre::Camera *left, Ogre::Camera *right);
		virtual void SetRiftSceneNode(RiftSceneNode *riftSceneNode);

	private:
		Rift *rift;
		Ogre::Root *root;
		Ogre::SceneManager *riftSceneManager;
		Ogre::TexturePtr renderTexture[2];
};

}; // ARLib namespace

#endif
