#ifndef CANNON_H
#define CANNON_H

#include "OGRE/Ogre.h"

class StarWarsLaserCannon{
public:
    StarWarsLaserCannon(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr);
    ~StarWarsLaserCannon();

private:
    Ogre::SceneNode *mSceneNode;
};

#endif //CANNON_H
