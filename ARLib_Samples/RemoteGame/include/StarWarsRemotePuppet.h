#ifndef REMOTE_PUPPET_H
#define REMOTE_PUPPET_H

#include "OGRE/OgreVector3.h"
#include "OGRE/OgreAxisAlignedBox.h"

namespace Ogre
{
	class SceneManager;
	class SceneNode;
};

namespace OgreBulletDynamics
{
	class DynamicsWorld;
};

class StarWarsRemote;

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

	Ogre::Vector3 mCenterOfRotation;

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
