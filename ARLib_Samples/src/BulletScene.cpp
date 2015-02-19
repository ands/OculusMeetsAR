#include "BulletScene.h"
#include "RigidListenerNode.h"
#include "LaserBulletManager.h"
#include "GlowMaterialListener.h"

#define VISIBILITY_FLAG_LEFT  (1 << 0)
#define VISIBILITY_FLAG_RIGHT (1 << 1)

BulletScene::BulletScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow, Ogre::SceneManager *sceneMgr,
    OgreBulletDynamics::DynamicsWorld *dyWorld, 
    OIS::Mouse *mouse, OIS::Keyboard *keyboard)
    : mRenderTarget(nullptr)
    , mSmallRenderTarget(nullptr)
    , mToggle(true)
{
    LaserBulletManager::getSingleton().setDynamicsWorld(dyWorld);

    mGlow[0] = nullptr;
    mGlow[1] = nullptr;
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = sceneMgr;
    mDynamicsWorld = dyWorld;

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
	mSceneMgr->setShadowFarDistance(30);

	//rift node
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 0); // TODO: set correct rigid body id!
	mRiftNode->getBodyNode()->setPosition(3.0f, 1.5f, 3.0f);
	
    //mRiftNode->getBodyNode()->lookAt(Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD);
	//if (tracker)
	//	tracker->addRigidBodyEventListener(mRiftNode);
    
    //create viewports
    if(window && rift){
        mRenderTarget = new ARLib::RiftRenderTarget(rift, root, window);
        setRenderTarget(mRenderTarget);
    }

    if(smallWindow){
        mSmallRenderTarget = new ARLib::DebugRenderTarget(smallWindow);
        setRenderTarget(mSmallRenderTarget);
    }

    //add Glow compositor
    mGlow[0] = Ogre::CompositorManager::getSingleton().addCompositor(mRiftNode->getLeftCamera()->getViewport(), "Glow");
    mGlow[0]->setEnabled(mToggle);
    mGlow[1] = Ogre::CompositorManager::getSingleton().addCompositor(mRiftNode->getRightCamera()->getViewport(), "Glow");
    mGlow[1]->setEnabled(mToggle);
    GlowMaterialListener *gml = new GlowMaterialListener();
    Ogre::MaterialManager::getSingleton().addListener(gml);

    //room
	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");
	Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	roomEnt->setCastShadows( false );
	mRoomNode->attachObject( roomEnt );

    //roomLight
	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30.f );
	roomLight->setAttenuation( 65.0f, 1.0f, 0.07f, 0.017f );
	roomLight->setSpecularColour( .25f, .25f, .25f );
	roomLight->setDiffuseColour( 0.85f, 0.76f, 0.7f );
	roomLight->setPosition( 5.f, 5.f, 5.f );
	mRoomNode->attachObject( roomLight );


    //ground-plane
    OgreBulletCollisions::CollisionShape *shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0.15f,0.9f,0.0f), -5.0f);
    mShapes.push_back(shape);
    OgreBulletDynamics::RigidBody *planeBody = new OgreBulletDynamics::RigidBody("GroundPlane", mDynamicsWorld);
    planeBody->setStaticShape(shape, 0.1f, 0.8f);
    mRigidBodies.push_back(planeBody);
	
    //1st cube
	Ogre::SceneNode* cubeNode1 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt1 = mSceneMgr->createEntity( "cube.mesh" );
	cubeEnt1->getSubEntity(0)->setMaterialName( "CubeMaterialRed" );
	cubeNode1->attachObject( cubeEnt1 );
	cubeNode1->setPosition( -1.0, 0.0, 0.0 );
	cubeNode1->setScale( 0.5, 0.5, 0.5 );
    Ogre::AxisAlignedBox cubeBB = cubeEnt1->getBoundingBox();
    Ogre::Vector3 size = (cubeBB.getSize()/4.0f);
    
    OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
    OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody("defaultBox3",mDynamicsWorld);
    defaultBody->setShape(cubeNode1, sceneBoxShape, 0.6f, 0.6f, 1.0f, Ogre::Vector3(-1.0, 0.0, 0.0));

    mRemote = new StarWarsRemote(mRoomNode, mSceneMgr, mDynamicsWorld, mRiftNode->getHeadNode(),5.0f);
    mRemotePuppet = new StarWarsRemotePuppet(mRemote, mRiftNode->getBodyNode(), mSceneMgr->getRootSceneNode(), mSceneMgr, mDynamicsWorld, 10.0f);
    mRemotePuppet->init(mRiftNode->getHeadNode()->_getDerivedOrientation() * Ogre::Vector3(0,0,-1));

    RigidListenerNode *mSwordParentNode = new RigidListenerNode(mSceneMgr->getRootSceneNode(), mSceneMgr, 1);
    mSword = new StarWarsLightSaber(mSwordParentNode->getSceneNode(), mSceneMgr, mDynamicsWorld);
    if(tracker){
        tracker->addRigidBodyEventListener(mSwordParentNode);
    }

    //rift light
	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65.0f, 1.0f, 0.07f, 0.017f );
	light->setSpecularColour( .25f, .25f, .25f );
	light->setDiffuseColour( 0.35f, 0.27f, 0.23f );
	mRiftNode->getBodyNode()->attachObject(light);

    //Add Screenspace Ambient Occlusion
    mDebugLeftSSAO = new PFXSSAO(smallWindow, mRiftNode->getLeftCamera());
    mDebugRightSSAO = new PFXSSAO(smallWindow, mRiftNode->getRightCamera());
    mLeftSSAO = new PFXSSAO(window, mRiftNode->getLeftCamera());
    mRightSSAO = new PFXSSAO(window, mRiftNode->getRightCamera());
}

BulletScene::~BulletScene()
{
    if(mRenderTarget) delete mRenderTarget;
    if(mSmallRenderTarget) delete mSmallRenderTarget;

	mRoot->destroySceneManager(mSceneMgr);

    std::deque<OgreBulletDynamics::RigidBody*>::iterator itBody = mRigidBodies.begin();
    while(itBody != mRigidBodies.end()){
        delete *itBody;
        ++itBody;
    }
    mRigidBodies.clear();
    std::deque<OgreBulletCollisions::CollisionShape*>::iterator itShapes = mShapes.begin();
    while(itShapes != mShapes.end()){
        delete *itShapes;
        ++itShapes;
    }
    mShapes.clear();
	delete mRiftNode;
}

void BulletScene::setRenderTarget(ARLib::RenderTarget *renderTarget)
{
	mRiftNode->removeAllRenderTargets();
	mRiftNode->addRenderTarget(renderTarget);
	mRiftNode->getLeftCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_LEFT);
	mRiftNode->getRightCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_RIGHT);
}

void BulletScene::toggleGlow()
{
    mToggle = !mToggle;
    mGlow[0]->setEnabled(mToggle);
    mGlow[1]->setEnabled(mToggle);
}

void BulletScene::update(float dt)
{
	ARLib::Rift *rift = mRiftNode->getRift();
    mRemotePuppet->update(dt);
	mRemote->update(dt);
    mSword->update(dt);
    LaserBulletManager::getSingleton().update(dt);
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool BulletScene::keyPressed( const OIS::KeyEvent& e )
{
    if(e.key == OIS::KC_C){
        mRemotePuppet->init(mRiftNode->getHeadNode()->_getDerivedOrientation() * Ogre::Vector3(0,0,-1));
    }if(e.key == OIS::KC_V){
        mSword->draw();
    }if(e.key == OIS::KC_N){
        toggleGlow();
    }if(e.key == OIS::KC_D){
        mDynamicsWorld->setShowDebugShapes(!mDynamicsWorld->getShowDebugShapes());
    }
	return true;
}
bool BulletScene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool BulletScene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mRiftNode->setYaw(Ogre::Degree(-0.3f*e.state.X.rel));
		mRiftNode->setPitch(Ogre::Degree(-0.3f*e.state.Y.rel));
	}
	return true;
}
bool BulletScene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool BulletScene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}