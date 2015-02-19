#ifndef CANNON_H
#define CANNON_H

#include "OGRE/Ogre.h"

class StarWarsLaserCannon{
public:
    StarWarsLaserCannon(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr);
    ~StarWarsLaserCannon();

    void update(float dt);
    void shoot(const Ogre::Vector3& tarWorldPos);
private:
    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode *mSceneNode;
    Ogre::Vector3 mCannonCenters[8];
    Ogre::Vector3 mTargetWorldPos;
    unsigned int mShootingCannon;
    bool mShooting;
    float mAccumTime;
};

#endif //CANNON_H
