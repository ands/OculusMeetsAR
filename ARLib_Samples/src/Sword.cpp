#include "Sword.h"

StarWarsLightSaber::StarWarsLightSaber(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr)
    : mDrawn(false){
    mSceneNode = parentNode->createChildSceneNode("StarWarsLaserSword");
    Ogre::Entity *swordEntity = sceneMgr->createEntity("Blade.mesh");
    swordEntity->setMaterialName("SwordMaterial");
    mSceneNode->attachObject(swordEntity);
    mSceneNode->setScale(1.0f, 0.0f, 1.0f);

    //create bullet collision object

        
}

StarWarsLightSaber::~StarWarsLightSaber(){

}

void StarWarsLightSaber::draw(){
    if(!mDrawn){
        
    }else{
    
    }
}