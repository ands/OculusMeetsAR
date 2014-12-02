#include "Oculus/RiftNode.h"

namespace ARLib {

RiftNode::RiftNode(Ogre::SceneManager* sceneManager)
{
	mBodyNode = sceneManager->getRootSceneNode()->createChildSceneNode("BodyNode");
	mBodyNode->setFixedYawAxis(true); // don't roll!

	mBodyTiltNode = mBodyNode->createChildSceneNode();
	mHeadNode = mBodyTiltNode->createChildSceneNode("HeadNode");

	mCamLeft = sceneManager->createCamera("LeftCamera");
	mHeadNode->attachObject(mCamLeft);

	mCamRight = sceneManager->createCamera("RightCamera");
	mHeadNode->attachObject(mCamRight);

	// set camera defaults
	float ipd = 0.064f; // interpupillary distance
	mCamLeft->setPosition(-ipd/2.0f, 0.0f, 0.0f);
	mCamRight->setPosition(ipd/2.0f, 0.0f, 0.0f);

	mCamLeft->setFarClipDistance(50);
	mCamRight->setFarClipDistance(50);

	mCamLeft->setNearClipDistance(0.001f);
	mCamRight->setNearClipDistance(0.001f);
}

RiftNode::~RiftNode()
{
}

void RiftNode::setPose(Ogre::Quaternion orientation, Ogre::Vector3 position)
{
	mHeadNode->setOrientation(orientation);
	mHeadNode->setPosition(position);
}

void RiftNode::setIPD(float ipd)
{
	mCamLeft->setPosition(-ipd/2.0f, 0.0f, 0.0f);
	mCamRight->setPosition(ipd/2.0f, 0.0f, 0.0f);
}

void RiftNode::setPitch(Ogre::Radian angle)
{
	mBodyTiltNode->pitch(angle, Ogre::Node::TS_PARENT);
}

void RiftNode::setYaw(Ogre::Radian angle)
{
	mBodyNode->yaw(angle);
}

}; // ARLib namespace