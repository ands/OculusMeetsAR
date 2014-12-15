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

		// use one of the following to set the cameras for the render target:
		void SetCameras(Ogre::Camera *left, Ogre::Camera *right);
		void SetRiftSceneNode(RiftSceneNode *riftSceneNode);

	private:
		Ogre::RenderWindow *renderWindow;
};

}; // ARLib namespace

#endif
