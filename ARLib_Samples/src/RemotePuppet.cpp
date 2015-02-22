#include "RemotePuppet.h"
#include <random>

#define PIf 3.1415926f

StarWarsRemotePuppet::StarWarsRemotePuppet(StarWarsRemote *remote, Ogre::SceneNode* parentPosNode, Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, float radius)
    : mSceneNode(nullptr)
    , mRemote(remote)
    , mRadius(radius)
    , accumTime(0.0f)
    , mParentPosNode(parentPosNode)
    , mDistribution(-1.0f, 1.0f){
    mSceneNode = parentNode->createChildSceneNode("StarWarsRemotePuppet");
    Ogre::Entity *remoteEntity = sceneMgr->createEntity("cube.mesh");
    mSceneNode->attachObject(remoteEntity);
    mSceneNode->setScale(0.3f, 0.3f, 0.3f);
    
    mAcc = Ogre::Vector3(0.0f, 0.0f, 0.0f);
    mMaxVel = Ogre::Vector3(0.5f, 0.5f, 0.0f);
    mVel = Ogre::Vector3(0.0f, 0.0f, 0.0f);

    //valid Range [0, pi/2]x[0, 3]x[mRadius, mRadius]
    mMovingBounds = Ogre::AxisAlignedBox(0.0f, 0.0f, mRadius, PIf/2.0f, 3.0f, mRadius);
}

StarWarsRemotePuppet::~StarWarsRemotePuppet(){

}


void StarWarsRemotePuppet::update(float dt){
    /*accumTime += dt;
    if(accumTime > mTravelTime){
        accumTime = 0.0f;
        pickNewDestination();
    }
    Ogre::Vector3 newPos = mSceneNode->getPosition() + mVel*dt;
    Ogre::Vector2 newPosp = Ogre::Vector2(std::min(std::max(newPos.x, mMovingBounds.getMinimum().x), mMovingBounds.getMaximum().x),
                            std::min(std::max(newPos.y, mMovingBounds.getMinimum().y), mMovingBounds.getMaximum().y));
                            
    mSceneNode->setPosition(newPosp.x, newPosp.y, mRadius);

    mRemote->changePos(Ogre::Vector3(mRadius*std::cosf(newPosp.x), newPosp.y, mRadius*std::sinf(newPosp.x))+mParentPosNode->_getDerivedPosition(), mParentPosNode->_getDerivedOrientation());
    mRemote->changeMaterial(mVel.length()/mMaxVel.length());
    
    mVel = mVel + mAcc * dt;
    mVel.x = std::min(std::max(mVel.x, -mMaxVel.x), mMaxVel.x);
    mVel.y = std::min(std::max(mVel.y, -mMaxVel.y), mMaxVel.y);*/
}

void StarWarsRemotePuppet::pickNewDestination(){
    //some random stuff
    float pdx, pdy, mdx, mdy;
    Ogre::Vector2 dir = Ogre::Vector2(mSceneNode->getPosition().x, mSceneNode->getPosition().y).normalisedCopy();
    float xDir = dir.dotProduct(Ogre::Vector2(1,0));
    float yDir = dir.dotProduct(Ogre::Vector2(0,1));
    float mXDir = dir.dotProduct(Ogre::Vector2(-1,0));
    float mYDir = dir.dotProduct(Ogre::Vector2(0,-1));

    pdx = std::abs(mMovingBounds.getMinimum().x - mSceneNode->getPosition().x);
    mdx = std::abs(mMovingBounds.getMaximum().x - mSceneNode->getPosition().x);
    pdy = std::abs(mMovingBounds.getMinimum().y - mSceneNode->getPosition().y);
    mdy = std::abs(mMovingBounds.getMaximum().y - mSceneNode->getPosition().y);

    mAcc = Ogre::Vector3(mDistribution(mGenerator)*(PIf/2.0f), mDistribution(mGenerator)*3.0f, 0.0f);
    mAcc.normalise();
    mTravelTime = 0.0f;
    float factor = 0.5f;
    if(xDir > 0.0f && mdx != 0.0f){
        mAcc.x *= mdx;
        mTravelTime += mdx * factor;
    }else if(pdx != 0.0f){
        mAcc.x *= pdx;
        mTravelTime += pdx * factor;
    }
    if(yDir > 0.0f && mdy != 0.0f){
        mAcc.y *= mdy;
        mTravelTime *= mdy * factor;
    }else if(pdy != 0.0f){
        mAcc.y *= pdy;
        mTravelTime *= pdy * factor;
    }
    mTravelTime = std::abs(mTravelTime);
    mAcc.normalise();
    printf("%fs TravelTime, Destination: %f, %f, %f \n", mTravelTime, mAcc.x, mAcc.y, mAcc.z);
}

void StarWarsRemotePuppet::init(const Ogre::Vector3& lookDir){
    pickNewDestination();
    mVel = 0.0f;
    accumTime = 0.0f;
    mLookDir = lookDir;
    mSceneNode->setPosition(std::atan2f(lookDir.z, lookDir.x) + mParentPosNode->_getDerivedPosition().x,
                            lookDir.z + mParentPosNode->_getDerivedPosition().y, mRadius + mParentPosNode->_getDerivedPosition().z);

    mRemote->changePos(Ogre::Vector3(mRadius*std::cosf(mSceneNode->getPosition().x), mSceneNode->getPosition().y, mRadius*std::sinf(mSceneNode->getPosition().x))+mParentPosNode->_getDerivedPosition(), mParentPosNode->_getDerivedOrientation());
}