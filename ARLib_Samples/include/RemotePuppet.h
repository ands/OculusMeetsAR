#ifndef REMOTE_PUPPET_H
#define REMOTE_PUPPET_H

#include "OGRE/Ogre.h"
#include "Remote.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include <random>

class StarWarsRemotePuppet{
public:
    StarWarsRemotePuppet(StarWarsRemote *remote, Ogre::SceneNode* parentPosNode, Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, float radius);
    ~StarWarsRemotePuppet();
		
    void update(float dt);

    void fire(const Ogre::Vector3& target);
    void pickNewDestination();
    void init(const Ogre::Vector3& position);
private:
    Ogre::SceneNode *mSceneNode;
    Ogre::SceneNode *mParentPosNode;

    std::default_random_engine mGenerator;
    std::uniform_real_distribution<float> mDistribution;

    Ogre::AxisAlignedBox mMovingBounds;
    StarWarsRemote *mRemote;

	float mRadius;
	float mYVel;
	float mXVel;
	const float mYMaxVel;
	const float mXMaxVel;
	float mYAcc;
	float mXAcc;

	float mTravelTimeX;
	float mTravelTimeY;

	const float mMaxTravelTimeX;
	const float mMaxTravelTimeY;
};

#endif
