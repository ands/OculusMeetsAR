#include "Remote.h"


StarWarsRemote::StarWarsRemote(Ogre::SceneNode *parentNode, Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dynamicsWorld, Ogre::SceneNode *player, float radius)
    : mSceneNode(nullptr)
    , mRemoteSphere(nullptr)
    , mRemoteBody(nullptr)
    , mRadius(radius)
    , mPlayer(player)
    , mAngularAccel(1.0f)
    , mAngularVelo(0.0f)
    , mMaxVelo(1.5f)
	, mTimeSinceShotsFired(0.0f)
	, mTimeBetweenShots(5.0f)
    , mTravelDest(180.0f,0.0f,radius){

    mSphericalPos.x = mRadius;
    mSphericalPos.y = std::acosf(mRadius/mSphericalPos.x);
    mSphericalPos.z = std::atan2f(mSphericalPos.y,mSphericalPos.x);
    
    mAngularVelo.x = 0.0f;
    mAngularAccel.x = 0.0f;

    mSceneNode = parentNode->createChildSceneNode("StarWarsRemote");
    mSceneNode->setInheritOrientation(false);
	mSceneNode->setPosition(0,0,-5);

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

    //mRemoteSphere = new OgreBulletCollisions::SphereCollisionShape(0.05f);
    //mRemoteBody = new OgreBulletDynamics::RigidBody("remoteBody", dynamicsWorld);
    //mRemoteBody->setShape(mSceneNode, mRemoteSphere, 0.6f, 0.6f, 1.0f, Ogre::Vector3(0.0f, 0.0f, 0.0f));

    mAccumTime = 0.0f;
    mAccumRot = 0.0f;
}

StarWarsRemote::~StarWarsRemote(){
    delete mRemoteSphere;
    delete mRemoteBody;
}

void StarWarsRemote::update(float dt){
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
			//pick new random time
			mCannons->shoot(mPlayer->_getDerivedPosition() + mPlayer->_getDerivedOrientation() * mPlayer->_getDerivedScale() * Ogre::Vector3::ZERO);
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

    //update Cannons
    mCannons->update(dt);
}

void StarWarsRemote::changePos(const Ogre::Vector3& newPos, const Ogre::Quaternion& quat){
    mSceneNode->setPosition(newPos);
   // mSpinNode->setOrientation(quat);
}

void StarWarsRemote::changeMaterial(float interp){
    mThrusterIntermediatePass->setAmbient(mThrusterHighPass->getAmbient()*(1-interp) + mThrusterLowPass->getAmbient() * interp);
    mThrusterIntermediatePass->setDiffuse(mThrusterHighPass->getDiffuse()*(1-interp) + mThrusterLowPass->getDiffuse() * interp);
    mThrusterIntermediatePass->setSpecular(mThrusterHighPass->getSpecular()*(1-interp) + mThrusterLowPass->getSpecular() * interp);
    mThrusterIntermediatePass->setEmissive(mThrusterHighPass->getEmissive()*(1-interp) + mThrusterLowPass->getEmissive() * interp);
}
