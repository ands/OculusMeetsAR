#ifndef RENDERTARGET_H
#define RENDERTARGET_H

namespace Ogre
{
class Camera;
}; // Ogre namespace

namespace ARLib {
class RiftSceneNode;

class RenderTarget
{
	public:
		// use one of the following to set the cameras for the render target:
		virtual void SetCameras(Ogre::Camera *left, Ogre::Camera *right) = 0;
		virtual void SetRiftSceneNode(RiftSceneNode *riftSceneNode) = 0;
};

}; // ARLib namespace

#endif
