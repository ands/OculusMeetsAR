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

    mSceneNode->setPosition(mSphericalPos.x * std::sin(mSphericalPos.y) * std::cos(mSphericalPos.z),
                            mSphericalPos.x * std::sin(mSphericalPos.y) * std::sin(mSphericalPos.z),
                            mSphericalPos.x * std::cos(mSphericalPos.y));

    mRemoteSphere = new OgreBulletCollisions::SphereCollisionShape(1.0f);
    mRemoteBody = new OgreBulletDynamics::RigidBody("remoteBody", dynamicsWorld);
    mRemoteBody->setShape(mSceneNode, mRemoteSphere, 0.6f, 0.6f, 1.0f, Ogre::Vector3(-1.0f, 0.0f, 0.0f));
}

StarWarsRemote::~StarWarsRemote(){
    delete mRemoteSphere;
    delete mRemoteBody;
}

bool StarWarsRemote::frameStarted(const Ogre::FrameEvent& evt){
    (void)evt;
    return true;
}

bool StarWarsRemote::frameRenderingQueued(const Ogre::FrameEvent& evt){
    if(mTravelDest == mSceneNode->getPosition()){
        if(mShotsFired == false){
            fire(Ogre::Vector3(0.0f));
        }
        pickNewDestination();
    }
    else{
        float invert = 1.0f;
        if((mTravelDest).distance(mSphericalPos) < 5.0f){
            invert = -invert;
        }
        Ogre::Vector3 dir = (mTravelDest - mSphericalPos).normalisedCopy();
        mSphericalPos.y = std::fmodf(mSphericalPos.y + invert * dir.y * mAngularVelo.y * evt.timeSinceLastFrame, 360.f);
        mSphericalPos.z = std::fmodf(mSphericalPos.z + invert * dir.z * mAngularVelo.z * evt.timeSinceLastFrame, 360.f);

        mAngularVelo.y = std::min(mAngularVelo.y + mAngularAccel.y * evt.timeSinceLastFrame, mMaxVelo);
        mAngularVelo.z = std::min(mAngularVelo.z + mAngularAccel.z * evt.timeSinceLastFrame, mMaxVelo);
        
        mSceneNode->setPosition(mSphericalPos.x * std::sin(mSphericalPos.y) * std::cos(mSphericalPos.z),
                                mSphericalPos.x * std::sin(mSphericalPos.y) * std::sin(mSphericalPos.z),
                                mSphericalPos.x * std::cos(mSphericalPos.y));
    }
    return true;
}

bool StarWarsRemote::frameEnded(const Ogre::FrameEvent& evt){
    (void) evt;
    return true;
}

void StarWarsRemote::fire(const Ogre::Vector3& target){
    mShotsFired = true;
}

void StarWarsRemote::pickNewDestination(){
    //some random stuff

}