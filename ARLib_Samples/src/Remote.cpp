#include "Remote.h"


StarWarsRemote::StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::Vector3 normalDirection, float radius)
    : mSceneNode(nullptr)
    , mRemoteSphere(nullptr)
    , mRemoteBody(nullptr)
    , mRadius(radius)
    , mNormalDirection(normalDirection)
    , mShotsFired(false)
    , mAngularAccel(1.0f)
    , mAngularVelo(0.0f)
    , mMaxVelo(1.5f)
    , mTravelDest(180.0f,0.0f,radius){

    mSphericalPos.x = mRadius;
    mSphericalPos.y = std::acosf(mRadius/mSphericalPos.x);
    mSphericalPos.z = std::atan2f(mSphericalPos.y,mSphericalPos.x);
    
    mAngularVelo.x = 0.0f;
    mAngularAccel.x = 0.0f;

    mSceneNode = parentNode->createChildSceneNode("StarWarsRemote");
    Ogre::Entity *remoteEntity = sceneMgr->createEntity("cube.mesh");
    mSceneNode->attachObject(remoteEntity);
    mSceneNode->setInheritOrientation(false);


    mRemoteSphere = new OgreBulletCollisions::SphereCollisionShape(1.0f);
    mRemoteBody = new OgreBulletDynamics::RigidBody("remoteBody", dynamicsWorld);
    mRemoteBody->setShape(mSceneNode, mRemoteSphere, 0.6f, 0.6f, 1.0f, Ogre::Vector3(0.0f, -2.0f, 0.0f));

    mRemoteBody->setLinearVelocity(0,0,0);
}

StarWarsRemote::~StarWarsRemote(){
    delete mRemoteSphere;
    delete mRemoteBody;
}


void StarWarsRemote::update(float dt){
   
}

void StarWarsRemote::fire(const Ogre::Vector3& target){
    mShotsFired = true;
}

void StarWarsRemote::pickNewDestination(){
    //some random stuff

}