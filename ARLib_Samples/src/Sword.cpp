#include "Sword.h"
#include "OgreBullet/Collisions/Utils/OgreBulletCollisionsMeshToShapeConverter.h"

StarWarsLightSaber::StarWarsLightSaber(Ogre::SceneNode *parentNode, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * dynamicsWorld)
    : mDynamicsWorld(dynamicsWorld)
    , mDrawn(false)
    , mDrawing(false)
    , mAccumTime(0.0f){
    mSceneNode = parentNode->createChildSceneNode("StarWarsLaserSword");
    Ogre::Entity *swordEntity = sceneMgr->createEntity("Blade.mesh");
    swordEntity->setMaterialName("BlueGlowBig");
    mSceneNode->attachObject(swordEntity);
    mSceneNode->setScale(1.0f, 0.0f, 1.0f);

	mSoundNode = mSceneNode->createChildSceneNode();
	mSoundNode->setPosition(0.0f, 1.0f, 0.0f);
	
	ARLib::SoundManager::instance().getSound("../../media/StarWarsRemote/Sounds/lightsaber activation.wav");
	mSoundSource = new ARLib::SoundSource();

    //create bullet collision object
    OgreBulletCollisions::StaticMeshToShapeConverter *stmc = new OgreBulletCollisions::StaticMeshToShapeConverter();
    stmc->addEntity(swordEntity);
    mSwordShape = stmc->createConvex();
    mNoShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.0f, 0.0f, 0.0f));

    mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld, 2, 1);
    mSwordBody->setShape(mSceneNode, mNoShape, 0.6f, 0.6f, 0.0f);

	mSwordBody->getBulletRigidBody()->setUserPointer(nullptr);

    delete stmc;
}

StarWarsLightSaber::~StarWarsLightSaber(){
    delete mSwordBody;
    delete mNoShape;
    delete mSwordShape;
}

void StarWarsLightSaber::draw(){
    mDrawing = true;
}

void StarWarsLightSaber::update(float dt){
	mSoundSource->setPosition(mSoundNode->_getDerivedPosition());

    const float drawTime = 0.15f;
    mAccumTime += dt;
    if(mDrawing){
        if(!mDrawn){
			if(!mSoundSource->isPlaying())
				mSoundSource->playSound("../../media/StarWarsRemote/Sounds/lightsaber activation.wav");
            if(mAccumTime > drawTime){
                mAccumTime = 0.0f;
                mDrawn = true;
                mDrawing = false;

                delete mSwordBody;
                mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld);
                mSwordBody->setShape(mSceneNode, mSwordShape, 0.6f, 0.6f, 0.0f);
                mSceneNode->setScale(1.0f, 1.0f, 1.0f);
            }else{
                mSceneNode->setScale(1.0f, mAccumTime/drawTime, 1.0f);
            }
        }else{
            if(mAccumTime > drawTime){
                mAccumTime = 0.0f;
                mDrawn = false;
                mDrawing = false;

                delete mSwordBody;
                mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld);
                mSwordBody->setShape(mSceneNode, mNoShape, 0.6f, 0.6f, 0.0f);
                mSceneNode->setScale(1.0f, 0.0f, 1.0f);
            }else{
                mSceneNode->setScale(1.0f, (drawTime - mAccumTime)/drawTime, 1.0f);
            }
        }
    }
}