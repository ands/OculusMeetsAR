#include "StarWarsRemote.h"


StarWarsRemote::StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::SceneNode *player, float radius)
    : mSceneNode(nullptr)
    , mRemoteSphere(nullptr)
    , mRemoteBody(nullptr)
    , mRadius(radius)
    , mPlayer(player)
	, mTimeSinceShotsFired(0.0f)
	, mTimeBetweenShots(5.0f)
	, mDistribution(3.0f, 8.0f){
    mSceneNode = parentNode->createChildSceneNode("StarWarsRemote");
	mSceneNode->setScale(0.45f, 0.45f, 0.45f);
    mSceneNode->setInheritOrientation(false);

    mSpinNode = mSceneNode->createChildSceneNode("StarWarsRemoteSpin");
    mSpinNode->setFixedYawAxis(true);

    //Scene Construction
    const char* MeshNames[] = { "BigHatch.mesh",
                    "BlackHatch.mesh",
                    "BottomRing.mesh",
                    "ChromeWheel.mesh",
                    "GreebleBottom.mesh",
                    "GreebleTop.mesh",
                    "LargeRelief.mesh",
                    "Remote.mesh",
                    "SmallRelief.mesh",
                    "TankPart.mesh",
                    "Thruster.mesh",
                    "TopPart.mesh",
                    "UShape.mesh" };

    for(unsigned int i = 0; i < 13; i++){
        Ogre::SceneNode* mNode = mSpinNode->createChildSceneNode();
        Ogre::Entity *entity = sceneMgr->createEntity(MeshNames[i]);
        mNode->attachObject(entity);
    }

    mThrusterNode = mSpinNode->createChildSceneNode("Thruster");
    Ogre::Entity *entity = sceneMgr->createEntity("Thruster.mesh");
    entity->setMaterialName("Thruster_intermediate");
    mThrusterNode->attachObject(entity);

    mCannons = new StarWarsLaserCannon(mSpinNode, sceneMgr);

    Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
    mThrusterHighPass = materialManager->getByName("Thruster_high")->getTechnique(0)->getPass(0);
    mThrusterLowPass = materialManager->getByName("Thruster_low")->getTechnique(0)->getPass(0);
    mThrusterIntermediatePass = materialManager->getByName("Thruster_intermediate")->getTechnique(0)->getPass(0);

    mAccumTime = 0.0f;
    mAccumRot = 0.0f;

	mRemoteBody = new OgreBulletDynamics::RigidBody("StarWarsRemote", dynamicsWorld, 4, 1); //set CollisionMask!
	mRemoteSphere = new OgreBulletCollisions::SphereCollisionShape(0.225f);
	mRemoteBody->setShape(mSpinNode, mRemoteSphere, 0.6f, 0.6f, 0.0f);
}

StarWarsRemote::~StarWarsRemote(){
    delete mRemoteSphere;
    delete mRemoteBody;
}

void StarWarsRemote::update(float dt){
	Ogre::Vector3 p = mSpinNode->_getDerivedPosition();
	Ogre::Quaternion q = mSpinNode->_getDerivedOrientation();
	mRemoteBody->getBulletRigidBody()->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));

	const float GlowTime = 0.5f;
    mAccumTime += dt;
    mAccumRot += dt/2.0f;
	mTimeSinceShotsFired += dt;
	if(mTimeSinceShotsFired <= GlowTime + 0.1f && mTimeSinceShotsFired >= 0.1){
		mCannons->changeMaterial(1.0f - std::max(std::min((GlowTime - mTimeSinceShotsFired)/GlowTime, 1.0f),0.0f));
	}else if((mTimeBetweenShots - mTimeSinceShotsFired) <= GlowTime){
		mCannons->changeMaterial(1.0f - std::max(std::min((GlowTime - (mTimeBetweenShots - mTimeSinceShotsFired))/GlowTime, 1.0f), 0.0f));
		if(mTimeSinceShotsFired >= mTimeBetweenShots + 0.1f){
			mTimeSinceShotsFired = 0.0f;
			mTimeBetweenShots = mDistribution(mGenerator);
			mCannons->shoot(mPlayer->_getDerivedPosition() + mPlayer->_getDerivedOrientation() * mPlayer->_getDerivedScale() * Ogre::Vector3::ZERO + Ogre::Vector3(0,-0.15f, 0));
		}
    }
    mSpinNode->setOrientation(Ogre::Quaternion(Ogre::Radian(mAccumRot), Ogre::Vector3::UNIT_Y));
    if(mAccumTime >= 2.0f){
        mAccumTime = 0.0f;
        changeMaterial(0.0f);
    }
    if(mAccumTime >= 1.0f){
        changeMaterial(1.0f - mAccumTime);
    }else{
        changeMaterial(mAccumTime);
    }
}

void StarWarsRemote::changePos(const Ogre::Vector3& newPos){
    mSceneNode->setPosition(newPos);
}

void StarWarsRemote::changeMaterial(float interp){
    mThrusterIntermediatePass->setAmbient(mThrusterHighPass->getAmbient()*(1-interp) + mThrusterLowPass->getAmbient() * interp);
    mThrusterIntermediatePass->setDiffuse(mThrusterHighPass->getDiffuse()*(1-interp) + mThrusterLowPass->getDiffuse() * interp);
    mThrusterIntermediatePass->setSpecular(mThrusterHighPass->getSpecular()*(1-interp) + mThrusterLowPass->getSpecular() * interp);
    mThrusterIntermediatePass->setEmissive(mThrusterHighPass->getEmissive()*(1-interp) + mThrusterLowPass->getEmissive() * interp);
}
