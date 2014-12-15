#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/DebugRenderTarget.h"

namespace ARLib {

DebugRenderTarget::DebugRenderTarget(Ogre::RenderWindow *renderWindow)
	: renderWindow(renderWindow)
{
}

void DebugRenderTarget::SetCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	renderWindow->removeAllViewports();
	Ogre::Viewport* debugL = renderWindow->addViewport(left, 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Ogre::Viewport* debugR = renderWindow->addViewport(right, 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

void DebugRenderTarget::SetRiftSceneNode(RiftSceneNode *riftSceneNode)
{
	SetCameras(riftSceneNode->getLeftCamera(), riftSceneNode->getRightCamera());
}

}; // ARLib namespace