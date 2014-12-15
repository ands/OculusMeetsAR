#include "ARLib/Ogre/RiftSceneNode.h"

namespace ARLib {

RiftSceneNode::RiftSceneNode(Rift *rift, Ogre::SceneManager *sceneManager, float zNear, float zFar, unsigned int rigidBodyID)
	: RigidBodyEventListener(rigidBodyID)
	, rift(rift)
{
	// create a virtual body node that the rift is correctly attached to
	Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
	bodyNode = rootNode->createChildSceneNode("BodyNode");
	bodyNode->setFixedYawAxis(true);

	bodyTiltNode = bodyNode->createChildSceneNode();
	headNode = bodyTiltNode->createChildSceneNode("HeadNode");

	cameras[0] = sceneManager->createCamera( "LeftCamera");
	cameras[1] = sceneManager->createCamera("RightCamera");

	headNode->attachObject(cameras[0]);
	headNode->attachObject(cameras[1]);

	cameras[0]->setNearClipDistance(zNear);
	cameras[1]->setNearClipDistance(zNear);

	cameras[0]->setFarClipDistance(zFar);
	cameras[1]->setFarClipDistance(zFar);

	// initialize the interpupillary distance
	float ipd = 0.064f;
	if (rift)
		ipd = rift->getInterpupillaryDistance();
	cameras[0]->setPosition(-ipd/2.0f, 0.0f, 0.0f);
	cameras[1]->setPosition( ipd/2.0f, 0.0f, 0.0f);
}

RiftSceneNode::~RiftSceneNode()
{
}

void RiftSceneNode::setPitch(Ogre::Radian angle)
{
	bodyTiltNode->pitch(angle, Ogre::Node::TS_PARENT);
}

void RiftSceneNode::setYaw(Ogre::Radian angle)
{
	bodyNode->yaw(angle);
}

void RiftSceneNode::onChange(RigidBody *rb)
{
	headNode->setOrientation(rb->mOrientation);
	headNode->setPosition(rb->mPosition);
}

}; // ARLib namespace