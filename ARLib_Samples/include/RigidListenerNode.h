#ifndef RIGID_LISTENER_NODE_H
#define RIGID_LISTENER_NODE_H

#include "Ogre/Ogre.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

class RigidListenerNode : public ARLib::RigidBodyEventListener{
public:
	RigidListenerNode(Ogre::SceneNode *parent, Ogre::SceneManager *sceneManager, unsigned int rigidBodyID) 
        : ARLib::RigidBodyEventListener(rigidBodyID, false){
		mRigidBodyNode = parent->createChildSceneNode();
	};
	void onChange(const ARLib::RigidBody* rb){
		mRigidBodyNode->setOrientation(rb->mqW, rb->mqX, rb->mqY, rb->mqZ);
        mRigidBodyNode->setPosition(rb->mX, rb->mY, rb->mZ);
		//do other interresting stuff
	};
    Ogre::SceneNode *getSceneNode(){
        return mRigidBodyNode;
    }
private:
	Ogre::SceneNode *mRigidBodyNode;
};


#endif