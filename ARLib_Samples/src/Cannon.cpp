#include "Cannon.h"


StarWarsLaserCannon::StarWarsLaserCannon(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr){
    mSceneNode = parentNode->createChildSceneNode();

    for(unsigned int i = 1; i <= 8; i++){
        Ogre::Entity *entity = sceneMgr->createEntity("Cannon" + std::to_string(static_cast<unsigned long long>(i)) + ".mesh");
        mSceneNode->attachObject(entity);
    }
}

StarWarsLaserCannon::~StarWarsLaserCannon(){

}