#include "OGRE/OgreSceneManager.h"
#include "RigidListenerNode.h"

RigidListenerNode::RigidListenerNode(Ogre::SceneNode *parent, Ogre::SceneManager *sceneManager, unsigned int rigidBodyID) 
    : ARLib::RigidBodyEventListener(rigidBodyID, false)
{
	mRigidBodyNode = parent->createChildSceneNode();
	mRigidBodyOrientationNode = mRigidBodyNode->createChildSceneNode();
	mRigidBodyCalibNode = mRigidBodyOrientationNode->createChildSceneNode();
};

void RigidListenerNode::onChange(const ARLib::RigidBody* rb)
{
    mRigidBodyNode->setPosition(rb->mX, rb->mY, rb->mZ);
	mRigidBodyCalibNode->setOrientation(Ogre::Quaternion(mRefQW, mRefQX, mRefQY, mRefQZ).UnitInverse());
	mRigidBodyOrientationNode->setOrientation(rb->mqW, rb->mqX, rb->mqY, rb->mqZ);
	//do other interresting stuff
};

Ogre::SceneNode *RigidListenerNode::getSceneNode()
{
    return mRigidBodyCalibNode;
}