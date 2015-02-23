#ifndef SWORD_H
#define SWORD_H

#include "OGRE/Ogre.h"
#include "ARLib/Sound/SoundSource.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsConvexHullShape.h"

class StarWarsLightSaber{
public:
    StarWarsLightSaber(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * dynamicsWorl);
    ~StarWarsLightSaber();

    void draw();
    void update(float dt);
private:
    OgreBulletCollisions::BoxCollisionShape *mNoShape;
    OgreBulletCollisions::ConvexHullCollisionShape *mSwordShape;
    OgreBulletDynamics::RigidBody *mSwordBody;
    OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;

	ARLib::SoundSource* mSoundSource;

    Ogre::SceneNode *mSceneNode;
	Ogre::SceneNode *mSoundNode;
    float mAccumTime;
    bool mDrawn;
    bool mDrawing;
};

#endif
