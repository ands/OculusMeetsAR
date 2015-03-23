#ifndef SWORD_H
#define SWORD_H

#include "OGRE/OgreVector3.h"

namespace Ogre
{
	class SceneNode;
	class SceneManager;
};

namespace OgreBulletDynamics
{
	class DynamicsWorld;
	class RigidBody;
};

namespace OgreBulletCollisions
{
	class BoxCollisionShape;
	class ConvexHullCollisionShape;
};

namespace ARLib
{
	class SoundSource;
};

class StarWarsLightSaber
{
public:
    StarWarsLightSaber(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * dynamicsWorld);
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
