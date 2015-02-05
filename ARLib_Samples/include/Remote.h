#ifndef REMOTE_H
#define REMOTE_H

#include "OGRE/Ogre.h"
#include "Cannon.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"

class StarWarsRemote{
public:
    StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::Vector3 normalDirection, float radius);
    ~StarWarsRemote();
		
    void update(float dt);
    void changePos(const Ogre::Vector3& newPos, const Ogre::Quaternion& quat);
    void changeMaterial(float interp);

    void fire(const Ogre::Vector3& target);
    void pickNewDestination();
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

    Ogre::Vector3 mSphericalPos;
    Ogre::Vector3 mAngularAccel;
    Ogre::Vector3 mAngularVelo;
    float mMaxVelo;
    Ogre::Vector3 mTravelDest;

    Ogre::Vector3 mNormalDirection;
    float mRadius;
    float mAccumTime;
    bool mShotsFired;
};

#endif
