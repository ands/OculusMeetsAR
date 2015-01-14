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

	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");

	RigidListenerNode* cubeNodeT = new RigidListenerNode(mRoomNode, mSceneMgr);
	if (tracker)
		tracker->registerRigidBodyEventListener(cubeNodeT);

    OgreBulletCollisions::CollisionShape *shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0.15,0.9,0), -5);
    mShapes.push_back(shape);
    OgreBulletDynamics::RigidBody *planeBody = new OgreBulletDynamics::RigidBody("GroundPlane", mDynamicsWorld);
    planeBody->setStaticShape(shape, 0.1, 0.8);
    mRigidBodies.push_back(planeBody);
	
	Ogre::SceneNode* cubeNode3 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "cube.mesh" );
	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode3->attachObject( cubeEnt3 );
	cubeNode3->setPosition( -1.0, 0.0, 0.0 );
	cubeNode3->setScale( 0.5, 0.5, 0.5 );
    Ogre::AxisAlignedBox cubeBB = cubeEnt3->getBoundingBox();
    Ogre::Vector3 size = (cubeBB.getSize()/4.0f);
    
    OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
    OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody("defaultBox3",mDynamicsWorld);
    defaultBody->setShape(cubeNode3, sceneBoxShape, 0.6f, 0.6f, 1.0f, Ogre::Vector3(-1.0, 0.0, 0.0));
    
	Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	roomEnt->setCastShadows( false );
	mRoomNode->attachObject( roomEnt );

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.85, 0.76, 0.7 );

	roomLight->setPosition( 5, 5, 5 );

	mRoomNode->attachObject( roomLight );

	// rift node:
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 0); // TODO: set correct rigid body id!
	mRiftNode->getNode()->setPosition(4.0f, 1.5f, 4.0f);
	//mRiftNode->getNode()->lookAt(Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD);
	if (tracker)
		tracker->registerRigidBodyEventListener(mRiftNode);

    StarWarsRemote *remote = new StarWarsRemote(mRoomNode, mSceneMgr, mDynamicsWorld, Ogre::Vector3(-1,0,0),5.0f);
    mRoot->addFrameListener(remote);

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mRiftNode->getNode()->attachObject(light);
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
	if (rift)
	{
		// reset rift view on R
		if (mKeyboard->isKeyDown(OIS::KC_R))
			rift->recenterPose();

		// TODO: this needs to be done by the tracking system!
		static ARLib::RigidBody rb; float q[4]; float p[3];
		rift->getPose(p, q);
		rb.mqX = q[0];
		rb.mqY = q[1];
		rb.mqZ = q[2];
		rb.mqW = q[3];
		mRiftNode->onChange(&rb);
	}

	// TODO: will also be handled by the tracking system?
	/*float forward = (mKeyboard->isKeyDown( OIS::KC_W ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_S ) ? 0 : -1);
	float leftRight = (mKeyboard->isKeyDown( OIS::KC_A ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_D ) ? 0 : -1);

	if( mKeyboard->isKeyDown( OIS::KC_LSHIFT ) )
	{
		forward *= 3;
		leftRight *= 3;
	}
	
	Ogre::Vector3 dirX = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_X;
	Ogre::Vector3 dirZ = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_Z;

	mBodyNode->setPosition( mBodyNode->getPosition() + dirZ*forward*dt + dirX*leftRight*dt );*/
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