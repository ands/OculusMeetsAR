#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/DebugRenderTarget.h"

namespace ARLib {

DebugRenderTarget::DebugRenderTarget(Ogre::RenderWindow *_renderWindow)
	: renderWindow(_renderWindow)
{
}

void DebugRenderTarget::setCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	renderWindow->removeAllViewports();
	Ogre::Viewport* debugL = renderWindow->addViewport(left, 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Ogre::Viewport* debugR = renderWindow->addViewport(right, 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

/*void DebugRenderTarget::setTimewarpMatrices(Ogre::Matrix4 &leftRotationStart , Ogre::Matrix4 &leftRotationEnd,
											Ogre::Matrix4 &rightRotationStart, Ogre::Matrix4 &rightRotationEnd)
{
}*/

}; // ARLib namespace