#include "LaserBulletManager.h"
#include "OgreBullet/Collisions/Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "OgreBullet/Dynamics/Constraints/OgreBulletDynamicsHingeConstraint.h"

LaserBullet::LaserBullet(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, const Ogre::Vector3& position, const Ogre::Vector3& impulse)
    : mShape(nullptr)
    , mBody(nullptr)
    , mLifeTime(0.0f){
    mSceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->setPosition(position);

    Ogre::Entity *bullet = sceneMgr->createEntity("Bullet.mesh");
    bullet->setMaterialName("RedGlow");
    mSceneNode->attachObject(bullet);
    
    //rotate Y axis onto impulse
    Ogre::Vector3 cross = Ogre::Vector3::UNIT_Y.crossProduct(impulse.normalisedCopy());
    Ogre::Matrix3 rotation = Ogre::Matrix3(0, cross.z, -cross.y, -cross.z, 0, cross.x, cross.y, -cross.x, 0);
    Ogre::Vector3 axis;
    Ogre::Degree angle;
    rotation.ToAngleAxis(axis, angle);

    OgreBulletCollisions::StaticMeshToShapeConverter *stmc = new OgreBulletCollisions::StaticMeshToShapeConverter();
    stmc->addEntity(bullet);

    mShape = stmc->createConvex();
    mBody = new OgreBulletDynamics::RigidBody("LaserBullet" + std::to_string(LaserBulletManager::getSingleton().getBulletCounter()), dyWorld);
    mBody->setShape(mSceneNode, mShape, 0.6f, 0.6f, 1.0f, Ogre::Vector3::ZERO, Ogre::Quaternion(Ogre::Radian(angle), axis));
    mBody->enableActiveState();
    mBody->applyImpulse(impulse, Ogre::Vector3::ZERO);

	//OgreBulletDynamics::HingeConstraint *lockedYAxis;
	//lockedYAxis = new OgreBulletDynamics::HingeConstraint(mBody, Ogre::Vector3(0,1,0), Ogre::Vector3(0,1,0));
	//dyWorld->addConstraint(lockedYAxis);
    delete stmc;
}

LaserBullet::~LaserBullet(){
    if(mBody != nullptr)
        delete mBody;
    if(mShape != nullptr)
        delete mShape;
	if(mSceneNode != nullptr)
		delete mSceneNode;
}

bool LaserBullet::update(float dt){
    const float MaxLifeTime = 30.0f;
    mLifeTime += dt;
    if(mLifeTime >= MaxLifeTime){
        return false;
    }
    return true;
}

LaserBulletManager::~LaserBulletManager(){
    for(std::deque<LaserBullet*>::iterator it = mShots.begin(); it != mShots.end(); ++it){
        //delete *it;
    }
}

void LaserBulletManager::update(float dt){
    unsigned int pop_backs = 0;
    for(std::deque<LaserBullet*>::iterator it = mShots.begin(); it != mShots.end(); ++it){
        if(!(*it)->update(dt)){
            delete (*it);
            pop_backs++;
        }
    }
    mShots.erase(mShots.begin(), mShots.begin() + pop_backs);
}

void LaserBulletManager::addBullet(LaserBullet* bullet){
    mBulletCounter ++;
    mShots.push_back(bullet);
}