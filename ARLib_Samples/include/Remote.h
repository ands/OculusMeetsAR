#ifndef REMOTE_H
#define REMOTE_H

#include "OGRE/Ogre.h"
#include "Cannon.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"

class StarWarsRemote{
public:
    StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::SceneNode* player, float radius);
    ~StarWarsRemote();
		
    void update(float dt);
    void changePos(const Ogre::Vector3& newPos, const Ogre::Quaternion& quat);
    void changeMaterial(float interp);

private:
    Ogre::SceneNode *mSceneNode;
    Ogre::SceneNode *mSpinNode;
    OgreBulletCollisions::SphereCollisionShape *mRemoteSphere;
    OgreBulletDynamics::RigidBody *mRemoteBody;

    Ogre::SceneNode *mThrusterNode;
    Ogre::Pass *mThrusterHighPass;
    Ogre::Pass *mThrusterLowPass;
    Ogre::Pass *mThrusterIntermediatePass;

    StarWarsLaserCannon *mCannons;

    Ogre::SceneNode *mPlayer;
    float mRadius;
    float mAccumTime;
    float mAccumRot;
	float mTimeSinceShotsFired;
	float mTimeBetweenShots;
};

#endif
