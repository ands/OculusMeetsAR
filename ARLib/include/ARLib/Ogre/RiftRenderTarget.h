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

		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

		// to use timewarp, this should be called on every update
		/*virtual void setTimewarpMatrices(
			Ogre::Matrix4 &leftRotationStart , Ogre::Matrix4 &leftRotationEnd,
			Ogre::Matrix4 &rightRotationStart, Ogre::Matrix4 &rightRotationEnd);*/

	private:
		Rift *rift;
		Ogre::Root *root;
		Ogre::SceneManager *riftSceneManager;
		Ogre::TexturePtr renderTexture[2];
		Ogre::MaterialPtr material[2];
};

}; // ARLib namespace

#endif
