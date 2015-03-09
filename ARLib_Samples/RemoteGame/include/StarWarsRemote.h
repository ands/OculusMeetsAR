#ifndef REMOTE_H
#define REMOTE_H

#include <random>

namespace Ogre
{
	class SceneManager;
	class SceneNode;
	class Pass;
};

namespace OgreBulletDynamics
{
	class DynamicsWorld;
	class RigidBody;
};

namespace OgreBulletCollisions
{
	class SphereCollisionShape;
};

class StarWarsLaserCannon;

class StarWarsRemote{
public:
    StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::SceneNode* player, float radius);
    ~StarWarsRemote();
		
    void update(float dt);
    void changePos(const Ogre::Vector3& newPos);
    void changeMaterial(float interp);

private:
    Ogre::SceneNode *mSceneNode;
    Ogre::SceneNode *mSpinNode;
    OgreBulletCollisions::SphereCollisionShape *mRemoteSphere;
    OgreBulletDynamics::RigidBody *mRemoteBody;
	
    std::default_random_engine mGenerator;
    std::uniform_real_distribution<float> mDistribution;

    Ogre::SceneNode *mThrusterNode;
    Ogre::Pass *mThrusterHighPass;
    Ogre::Pass *mThrusterLowPass;
    Ogre::Pass *mThrusterIntermediatePass;

    StarWarsLaserCannon *mCannons;

    Ogre::SceneNode *mPlayer;
    float mRadius;
    float mAccumTime;
    float mAccumRot;
	float mTimeSinceShotsFired;
	float mTimeBetweenShots;
};

#endif
