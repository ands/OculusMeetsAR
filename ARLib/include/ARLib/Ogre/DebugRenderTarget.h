#ifndef DEBUGRENDERTARGET_H
#define DEBUGRENDERTARGET_H

#include "OGRE/Ogre.h"
#include "ARLib/Ogre/RenderTarget.h"

namespace ARLib {

class RiftSceneNode;
	
class DebugRenderTarget : public RenderTarget
{
	public:
		DebugRenderTarget(Ogre::RenderWindow *renderWindow);

		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

		// to use timewarp, this should be called on every update
		/*virtual void setTimewarpMatrices(
			Ogre::Matrix4 &leftRotationStart , Ogre::Matrix4 &leftRotationEnd,
			Ogre::Matrix4 &rightRotationStart, Ogre::Matrix4 &rightRotationEnd);*/

	private:
		Ogre::RenderWindow *renderWindow;
};

}; // ARLib namespace

#endif
