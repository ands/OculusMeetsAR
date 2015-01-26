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
		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right) = 0;

		// to use timewarp, this should be called on every update
		/*virtual void setTimewarpMatrices(
			Ogre::Matrix4 &leftRotationStart , Ogre::Matrix4 &leftRotationEnd,
			Ogre::Matrix4 &rightRotationStart, Ogre::Matrix4 &rightRotationEnd) = 0;*/

	friend class RiftSceneNode;
};

}; // ARLib namespace

#endif
