#include "Cannon.h"
#include "LaserBulletManager.h"
#include <float.h>

StarWarsLaserCannon::StarWarsLaserCannon(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr)
    : mShooting(false)
    , mAccumTime(0.0f){
    mSceneNode = parentNode->createChildSceneNode();
    mSceneMgr = sceneMgr;
    for(unsigned int i = 0; i < 8; i++){
        Ogre::Entity *entity = sceneMgr->createEntity("Cannon" + std::to_string(static_cast<unsigned long long>(i+1)) + ".mesh");
        mSceneNode->attachObject(entity);
        mCannonCenters[i] = entity->getBoundingBox().getCenter();
    }
}

StarWarsLaserCannon::~StarWarsLaserCannon(){

}

void StarWarsLaserCannon::update(float dt){
    mAccumTime += dt;
    if(mShooting){
        if(mAccumTime >= 2.0f){
            mShooting = false;
            mAccumTime = 0.0f;
            Ogre::Vector3 shootFrom = mSceneNode->_getDerivedPosition() + mSceneNode->_getDerivedOrientation() * mSceneNode->_getDerivedScale() * mCannonCenters[mShootingCannon];
            LaserBulletManager::getSingleton().addBullet(
                        new LaserBullet(mSceneMgr
                      , LaserBulletManager::getSingleton().getDynamicsWorld()
                      , shootFrom
                      , (mTargetWorldPos - shootFrom) * 1.0f));
        }
    }else{
        mAccumTime = 0.0f;
    }
}


void StarWarsLaserCannon::shoot(const Ogre::Vector3& tarWorldPos){
    mShooting = true;
    mTargetWorldPos = tarWorldPos;
    Ogre::Vector3 minDistanceOrigin;
    Ogre::Vector3 inScenePos = mSceneNode->_getDerivedPosition();
    Ogre::Vector3 inSceneRotScale = mSceneNode->_getDerivedOrientation() * mSceneNode->_getDerivedScale();
    float minDist = FLT_MAX;
    mShootingCannon = 0;
    for(unsigned int i = 0; i < 8; i++){
        Ogre::Vector3 CannonCenter = inScenePos + inSceneRotScale * mCannonCenters[i];
        float distance = CannonCenter.distance(tarWorldPos);
        if(minDist > distance){
            minDist = distance;
            minDistanceOrigin = CannonCenter;
            mShootingCannon = i;
        }
    }
    printf("%f, %f, %f \n", minDistanceOrigin.x, minDistanceOrigin.y, minDistanceOrigin.z);
    /*LaserBulletManager::getSingleton().addBullet(
        new LaserBullet(mSceneMgr
                      , LaserBulletManager::getSingleton().getDynamicsWorld()
                      , minDistanceOrigin
                      , (tarWorldPos - minDistanceOrigin) * 1.0f));*/
}