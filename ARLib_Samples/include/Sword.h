#ifndef SWORD_H
#define SWORD_H

#include "OGRE/Ogre.h"
#include "ARLib/Tracking/RigidBodyEventListener.h"

class StarWarsLightSaber{
public:
    StarWarsLightSaber(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr);
    ~StarWarsLightSaber();

    void draw();
private:
    Ogre::SceneNode *mSceneNode;
    bool mDrawn;
};

#endif
