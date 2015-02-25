#include "RemotePuppet.h"
#include <random>

#define PIf 3.1415926f

StarWarsRemotePuppet::StarWarsRemotePuppet(StarWarsRemote *remote, Ogre::SceneNode* parentPosNode, Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, float radius)
    : mSceneNode(nullptr)
    , mRemote(remote)
    , mRadius(radius)
    , mParentPosNode(parentPosNode)
	, mXAcc(PIf/10.0f)
	, mXMaxVel(PIf/10.0f)
	, mYAcc(0.6f)
	, mYMaxVel(0.6f)
	, mMaxTravelTimeX(3.0f)
	, mMaxTravelTimeY(2.0f){
    mSceneNode = parentNode->createChildSceneNode("StarWarsRemotePuppet");
	mCenterOfRotation = mParentPosNode->_getDerivedPosition();
    //valid Range [0, pi/2]x[0, 3]x[mRadius, mRadius]
    mMovingBounds = Ogre::AxisAlignedBox(0.0f, 0.0f, mRadius, PIf/2.0f, 3.0f, mRadius);
}

StarWarsRemotePuppet::~StarWarsRemotePuppet(){

}


void StarWarsRemotePuppet::update(float dt){
	mTravelTimeX += dt;
	mTravelTimeY += dt;

	if(mTravelTimeX >= mMaxTravelTimeX){
		mTravelTimeX = 0.0f;
		mXAcc = -mXAcc;
	}
	if(mTravelTimeY >= mMaxTravelTimeY){
		mTravelTimeY = 0.0f;
		mYAcc = -mYAcc;
	}
	mSceneNode->setPosition(mSceneNode->getPosition().x + mXVel*dt, mSceneNode->getPosition().y + mYVel*dt, mSceneNode->getPosition().z);
	mRemote->changePos(Ogre::Vector3(mRadius*std::cosf(mSceneNode->getPosition().x), mSceneNode->getPosition().y, mRadius*std::sinf(mSceneNode->getPosition().x))+mCenterOfRotation);

	mXVel = std::min(std::max(mXVel + mXAcc * dt, -mXMaxVel), mXMaxVel);
	mYVel = std::min(std::max(mYVel + mYAcc * dt, -mYMaxVel), mYMaxVel);

	float value = Ogre::Vector2(mXVel, mYVel).length()/ Ogre::Vector2(mXMaxVel, mYMaxVel).length();
	mRemote->changeMaterial(value);
}

void StarWarsRemotePuppet::pickNewDestination(){
    //some random stuff
}

void StarWarsRemotePuppet::init(const Ogre::Vector3& position){
	mYVel = 0.0f;
	mXVel = 0.0f;
    mTravelTimeX = 1.5f;
	mTravelTimeY = 3.f;
	mXAcc = std::abs(mXAcc);
	mYAcc = -std::abs(mYAcc);

	mCenterOfRotation = mParentPosNode->_getDerivedPosition();
	mSceneNode->setPosition(std::atan2f(position.z, position.x) + mCenterOfRotation.x,
                            position.z + mCenterOfRotation.y, mRadius + mCenterOfRotation.z);
	
    mRemote->changePos(Ogre::Vector3(mRadius*std::cosf(mSceneNode->getPosition().x), mSceneNode->getPosition().y, mRadius*std::sinf(mSceneNode->getPosition().x))+mCenterOfRotation);
}