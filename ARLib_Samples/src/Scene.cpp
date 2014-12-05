#include "Scene.h"
#include "RigidListenerNode.h"

Scene::Scene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::RenderWindow *renderWindow, Ogre::Root *root, OIS::Mouse *mouse, OIS::Keyboard *keyboard)
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneMgr->setShadowFarDistance(30);

	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");

	RigidListenerNode* cubeNodeT = new RigidListenerNode(mRoomNode, mSceneMgr);
	tracker->registerRigidBodyEventListener(cubeNodeT);

	Ogre::SceneNode* cubeNode2 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition( 3.0, 0.0, 0.0 );
	cubeNode2->setScale( 0.5, 0.5, 0.5 );
	
	Ogre::SceneNode* cubeNode3 = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode3->attachObject( cubeEnt3 );
	cubeNode3->setPosition( -1.0, 0.0, 0.0 );
	cubeNode3->setScale( 0.5, 0.5, 0.5 );

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
	mRiftNode = new ARLib::RiftNode(rift, root, renderWindow, mSceneMgr, 0.001f, 50.0f, 0);
	mRiftNode->getNode()->setPosition(4.0f, 1.5f, 4.0f);
	//mRiftNode->getNode()->lookAt(Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD);

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mRiftNode->getNode()->attachObject(light);
}

Scene::~Scene()
{
	mRoot->destroySceneManager(mSceneMgr);
	delete mRiftNode;
}

void Scene::update(float dt)
{
	ARLib::Rift *rift = mRiftNode->getRift();
	if (rift)
	{
		// TODO: this needs to be done by the tracking system!
		static ARLib::RigidBody rb; float q[4];
		rift->getPose(&rb.mPosition.x, q);
		rb.mOrientation = Ogre::Quaternion(q[3], q[0], q[1], q[2]);
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

bool Scene::keyPressed( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mRiftNode->setYaw(Ogre::Degree(-0.3*e.state.X.rel));
		mRiftNode->setPitch(Ogre::Degree(-0.3*e.state.Y.rel));
	}
	return true;
}
bool Scene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool Scene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}