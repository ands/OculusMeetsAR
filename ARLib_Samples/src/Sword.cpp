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

	mSwordLocalPos = Ogre::Vector3(0,0,0); //delete!!!!

    //create bullet collision object
    OgreBulletCollisions::StaticMeshToShapeConverter *stmc = new OgreBulletCollisions::StaticMeshToShapeConverter(swordEntity);
    mSwordShape = stmc->createConvex();
	mSwordShape->getBulletShape()->setLocalScaling(btVector3(0.1f,0.95f,0.1f));
    mNoShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.0f, 0.0f, 0.0f));

    mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld, 2, 1);
	mSwordBody->setShape(mSceneNode, mNoShape, 0.6f, 0.6f, 0.0f, mSwordLocalPos);

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
	Ogre::Vector3 p = mSceneNode->_getDerivedPosition();
	Ogre::Quaternion q = mSceneNode->_getDerivedOrientation();
	mSwordBody->getBulletRigidBody()->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));

    const float drawTime = 0.15f;
    mAccumTime += dt;
    if(mDrawing){
        if(!mDrawn){
            if(mAccumTime > drawTime){
                mAccumTime = 0.0f;
                mDrawn = true;
                mDrawing = false;

                delete mSwordBody;
                mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld, 2, 1);
				mSwordBody->setShape(mSceneNode, mSwordShape, 0.6f, 0.6f, 0.0f, mSwordLocalPos);
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
                mSwordBody = new OgreBulletDynamics::RigidBody("SwordBody", mDynamicsWorld, 2, 1);
                mSwordBody->setShape(mSceneNode, mNoShape, 0.6f, 0.6f, 0.0f, mSwordLocalPos);
                mSceneNode->setScale(1.0f, 0.0f, 1.0f);
            }else{
                mSceneNode->setScale(1.0f, (drawTime - mAccumTime)/drawTime, 1.0f);
            }
        }
    }
}