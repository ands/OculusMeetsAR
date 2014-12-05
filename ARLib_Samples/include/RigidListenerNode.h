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
		mRigidBodyNode->setPosition( 1.0, 0.0, 0.0 );
		mRigidBodyNode->setScale( 0.5, 0.5, 0.5 );
	};
	void onChange(ARLib::RigidBody* rb){
		mRigidBodyNode->setOrientation(rb->mOrientation);
		//do other interresting stuff
	};
private:
	Ogre::SceneNode *mRigidBodyNode;
};


#endif