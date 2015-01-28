#include "BulletScene.h"
#include "RigidListenerNode.h"

BulletScene::BulletScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::SceneManager *sceneMgr,
    OgreBulletDynamics::DynamicsWorld *dyWorld, 
    OIS::Mouse *mouse, OIS::Keyboard *keyboard)
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = sceneMgr;
    mDynamicsWorld = dyWorld;

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
	mSceneMgr->setShadowFarDistance(30);

    //room
	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");
	Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	roomEnt->setCastShadows( false );
	mRoomNode->attachObject( roomEnt );

    //roomLight
	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.85, 0.76, 0.7 );
	roomLight->setPosition( 5, 5, 5 );
	mRoomNode->attachObject( roomLight );

	//RigidListenerNode* cubeNodeT = new RigidListenerNode(mRoomNode, mSceneMgr);
	//if (tracker)
	//	tracker->addRigidBodyEventListener(cubeNodeT);

    //ground-plane
    OgreBulletCollisions::CollisionShape *shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0.15,0.9,0), -5);
    mShapes.push_back(shape);
    OgreBulletDynamics::RigidBody *planeBody = new OgreBulletDynamics::RigidBody("GroundPlane", mDynamicsWorld);
    planeBody->setStaticShape(shape, 0.1, 0.8);
    mRigidBodies.push_back(planeBody);
	
    //1st cube
	Ogre::SceneNode* cubeNode1 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt1 = mSceneMgr->createEntity( "cube.mesh" );
	cubeEnt1->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode1->attachObject( cubeEnt1 );
	cubeNode1->setPosition( -1.0, 0.0, 0.0 );
	cubeNode1->setScale( 0.5, 0.5, 0.5 );
    Ogre::AxisAlignedBox cubeBB = cubeEnt1->getBoundingBox();
    Ogre::Vector3 size = (cubeBB.getSize()/4.0f);
    
    OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
    OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody("defaultBox3",mDynamicsWorld);
    defaultBody->setShape(cubeNode1, sceneBoxShape, 0.6f, 0.6f, 1.0f, Ogre::Vector3(-1.0, 0.0, 0.0));

	//rift node
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 0); // TODO: set correct rigid body id!
	mRiftNode->getBodyNode()->setPosition(3.0f, 1.5f, 3.0f);
	
    //mRiftNode->getBodyNode()->lookAt(Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD);
	//if (tracker)
	//	tracker->addRigidBodyEventListener(mRiftNode);

    mRemote = new StarWarsRemote(mRiftNode->getBodyNode(), mSceneMgr, mDynamicsWorld, Ogre::Vector3(-1,0,0),5.0f);
    
    //rift light
	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mRiftNode->getBodyNode()->attachObject(light);
}

BulletScene::~BulletScene()
{
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

void BulletScene::update(float dt)
{
	ARLib::Rift *rift = mRiftNode->getRift();
    mRemote->update(dt);
	
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool BulletScene::keyPressed( const OIS::KeyEvent& e )
{
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
		mRiftNode->setYaw(Ogre::Degree(-0.3*e.state.X.rel));
		mRiftNode->setPitch(Ogre::Degree(-0.3*e.state.Y.rel));
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