#include "Sword.h"

StarWarsLaserSword::StarWarsLaserSword(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr){
    mSceneNode = parentNode->createChildSceneNode("StarWarsLaserSword");
    Ogre::Entity *swordEntity = sceneMgr->createEntity("cube.mesh");
    swordEntity->setMaterialName("SwordMaterial");
    mSceneNode->attachObject(swordEntity);
    mSceneNode->setScale(0.3f, 1.0f, 0.3f);
    
}

StarWarsLaserSword::~StarWarsLaserSword(){

}