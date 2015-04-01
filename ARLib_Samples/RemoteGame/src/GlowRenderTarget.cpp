#include "OGRE/OgreCamera.h"
#include "OGRE/OgreCompositorManager.h"
#include "OGRE/OgreCompositorInstance.h"
#include "GlowRenderTarget.h"

GlowRenderTarget::GlowRenderTarget(ARLib::RenderTarget *_destination)
	: destination(_destination)
{
}

void GlowRenderTarget::setCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	camera[0] = left;
	camera[1] = right;
	destination->setCameras(camera[0], camera[1]);

	// attach compositor to both viewports
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		Ogre::Viewport *vp = camera[eyeNum]->getViewport();
		Ogre::CompositorInstance *glow = Ogre::CompositorManager::getSingleton().addCompositor(vp, "GlowBig");
		glow->setEnabled(true);
	}
}