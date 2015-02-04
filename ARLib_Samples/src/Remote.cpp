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
    mSceneNode->setInheritOrientation(false);

    mSpinNode = mSceneNode->createChildSceneNode("StarWarsRemoteSpin");
    Ogre::Entity *remoteEntity = sceneMgr->createEntity("cube.mesh");
    remoteEntity->setMaterialName("Thruster_intermediate");
    mSpinNode->attachObject(remoteEntity);
    mSpinNode->setFixedYawAxis(true);

    Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
    mThrusterHighPass = materialManager->getByName("Thruster_high")->getTechnique("Thruster_high_technique")->getPass("Thruster_high_pass");
    mThrusterLowPass = materialManager->getByName("Thruster_low")->getTechnique("Thruster_low_technique")->getPass("Thruster_low_pass");
    mThrusterIntermediatePass = materialManager->getByName("Thruster_intermediate")->getTechnique("Thruster_intermediate_technique")->getPass("Thruster_intermediate_pass");

    mRemoteSphere = new OgreBulletCollisions::SphereCollisionShape(1.0f);
    mRemoteBody = new OgreBulletDynamics::RigidBody("remoteBody", dynamicsWorld);
    mRemoteBody->setShape(mSpinNode, mRemoteSphere, 0.6f, 0.6f, 1.0f, Ogre::Vector3(0.0f, -2.0f, 0.0f));
}

StarWarsRemote::~StarWarsRemote(){
    delete mRemoteSphere;
    delete mRemoteBody;
}

void StarWarsRemote::update(float dt){
    if(mShotsFired){
        //animate remote and shoot lazors
    }
}

void StarWarsRemote::changePos(const Ogre::Vector3& newPos, const Ogre::Quaternion& quat){
    mRemoteBody->setPosition(newPos);
    mSpinNode->setOrientation(quat);
}

void StarWarsRemote::changeMaterial(float interp){
    mThrusterIntermediatePass->setAmbient(mThrusterHighPass->getAmbient()*(1-interp) + mThrusterLowPass->getAmbient() * interp);
    mThrusterIntermediatePass->setDiffuse(mThrusterHighPass->getDiffuse()*(1-interp) + mThrusterLowPass->getDiffuse() * interp);
    mThrusterIntermediatePass->setSpecular(mThrusterHighPass->getSpecular()*(1-interp) + mThrusterLowPass->getSpecular() * interp);
    mThrusterIntermediatePass->setEmissive(mThrusterHighPass->getEmissive()*(1-interp) + mThrusterLowPass->getEmissive() * interp);

    /*Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
    Ogre::MaterialPtr mp = materialManager->getByName("test");
    printf((std::string(mp->getParameter("diffuse").c_str()) + "\n").c_str());
    mp->getTechnique("test_technique")->getPass("test_standard")->setDiffuse(0,0,1,1);*/
}

void StarWarsRemote::fire(const Ogre::Vector3& target){
    mShotsFired = true;
}

void StarWarsRemote::pickNewDestination(){
    //some random stuff

}