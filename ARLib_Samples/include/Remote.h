#ifndef REMOTE_H
#define REMOTE_H

#include "OGRE/Ogre.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"

class StarWarsRemote : public Ogre::FrameListener{
public:
    StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::Vector3 normalDirection, float radius);
    ~StarWarsRemote();
    
    bool frameStarted(const Ogre::FrameEvent& evt);
		
    bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    bool frameEnded(const Ogre::FrameEvent& evt);

    void fire(const Ogre::Vector3& target);
    void pickNewDestination();
private:
    Ogre::SceneNode *mSceneNode;
    OgreBulletCollisions::SphereCollisionShape *mRemoteSphere;
    OgreBulletDynamics::RigidBody *mRemoteBody;

    Ogre::Vector3 mSphericalPos;
    Ogre::Vector3 mAngularAccel;
    Ogre::Vector3 mAngularVelo;
    float mMaxVelo;
    Ogre::Vector3 mTravelDest;

    Ogre::Vector3 mNormalDirection;
    float mRadius;
    bool mShotsFired;
};

#endif
