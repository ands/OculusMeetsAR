#ifndef RIGID_LISTENER_NODE_H
#define RIGID_LISTENER_NODE_H

#include "Ogre/Ogre.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

class RigidListenerNode : public ARLib::RigidBodyEventListener{
public:
	RigidListenerNode(Ogre::SceneNode *parent, Ogre::SceneManager *sceneManager, unsigned int rigidBodyID) 
        : ARLib::RigidBodyEventListener(rigidBodyID, false){
		mRigidBodyNode = parent->createChildSceneNode();
		mRigidBodyOrientationNode = mRigidBodyNode->createChildSceneNode();
		mRigidBodyCalibNode = mRigidBodyOrientationNode->createChildSceneNode();
	};
	void onChange(const ARLib::RigidBody* rb){
        mRigidBodyNode->setPosition(rb->mX, rb->mY, rb->mZ);
		mRigidBodyCalibNode->setOrientation(Ogre::Quaternion(mRefQW, mRefQX, mRefQY, mRefQZ).UnitInverse());
		mRigidBodyOrientationNode->setOrientation(rb->mqW, rb->mqX, rb->mqY, rb->mqZ);
		//do other interresting stuff
	};
    Ogre::SceneNode *getSceneNode(){
        return mRigidBodyCalibNode;
    }
private:
	Ogre::SceneNode *mRigidBodyNode;
	Ogre::SceneNode *mRigidBodyOrientationNode;
	Ogre::SceneNode *mRigidBodyCalibNode;
};


#endif