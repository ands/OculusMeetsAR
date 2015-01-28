#ifndef RIGID_LISTENER_NODE_H
#define RIGID_LISTENER_NODE_H

#include "Ogre/Ogre.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

class RigidListenerNode : public ARLib::RigidBodyEventListener{
public:
	RigidListenerNode(Ogre::SceneNode *parent, Ogre::SceneManager *sceneManager) : ARLib::RigidBodyEventListener(0){
		mRigidBodyNode = parent->createChildSceneNode();
		Ogre::Entity* cubeEnt = sceneManager->createEntity( "Cube.mesh" );
		cubeEnt->getSubEntity(0)->setMaterialName( "CubeMaterialRed" );
		mRigidBodyNode->attachObject( cubeEnt );
	};
	void onChange(ARLib::RigidBody* rb){
		mRigidBodyNode->setOrientation(rb->mqW, rb->mqX, rb->mqY, rb->mqZ);
		//do other interresting stuff
	};
    Ogre::SceneNode *getSceneNode(){
        return mRigidBodyNode;
    }
private:
	Ogre::SceneNode *mRigidBodyNode;
};


#endif