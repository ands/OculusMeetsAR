#ifndef SWORD_H
#define SWORD_H

#include "OGRE/Ogre.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

class StarWarsLaserSword{
public:
    StarWarsLaserSword(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr);
    ~StarWarsLaserSword();

private:
    Ogre::SceneNode *mSceneNode;
};

#endif
