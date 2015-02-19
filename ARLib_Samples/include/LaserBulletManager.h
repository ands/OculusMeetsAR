#ifndef LASER_BULLET_MANAGER_H
#define LASER_BULLET_MANAGER_H

#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"

class LaserBullet{
public:
    LaserBullet(Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, const Ogre::Vector3& position, const Ogre::Vector3& impulse);
    ~LaserBullet();
    bool update(float dt);
private:
    OgreBulletCollisions::ConvexHullCollisionShape *mShape;
    OgreBulletDynamics::RigidBody *mBody;
    Ogre::SceneNode *mSceneNode;
    float mLifeTime;
};

class LaserBulletManager{
public:
    static LaserBulletManager& getSingleton(){
        static LaserBulletManager mInstance;
        return mInstance;
    }
    ~LaserBulletManager();

    void setDynamicsWorld(OgreBulletDynamics::DynamicsWorld *dyWorld){
        mDynamicsWorld = dyWorld;
    }
    OgreBulletDynamics::DynamicsWorld* getDynamicsWorld(){
        return mDynamicsWorld;
    }
    unsigned long long getBulletCounter(){
        return mBulletCounter;
    }
    void update(float dt);
    void addBullet(LaserBullet*);
private:
    LaserBulletManager() : mBulletCounter(0){};
    LaserBulletManager(const LaserBulletManager& sm);
    LaserBulletManager& operator=(const LaserBulletManager& sm);

    unsigned long long mBulletCounter;
    OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;
    std::deque<LaserBullet*> mShots;
};

#endif
