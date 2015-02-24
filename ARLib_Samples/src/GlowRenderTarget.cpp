#include "GlowRenderTarget.h"

GlowRenderTarget::GlowRenderTarget(ARLib::RenderTarget *_destination)
	: destination(_destination)
{

}

GlowRenderTarget::~GlowRenderTarget()
{
}

void GlowRenderTarget::setCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	camera[0] = left;
	camera[1] = right;
	destination->setCameras(camera[0], camera[1]);

	// attach both cameras
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		Ogre::Viewport *vp = camera[eyeNum]->getViewport();
		// add blur compositor
		Ogre::CompositorInstance *glow = Ogre::CompositorManager::getSingleton().addCompositor(vp, "GlowBig");
		glow->setEnabled(true);
		
	}
}