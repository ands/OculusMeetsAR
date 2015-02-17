#include "WebcamScene.h"
#include "RigidListenerNode.h"
#include "NPRWatercolorRenderTarget.h"

#include "OGRE/Overlay/OgreOverlayManager.h"
#include "OGRE/Overlay/OgreOverlayContainer.h"
#include "OGRE/Overlay/OgreFontManager.h"

WebcamScene::WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::SceneManager *sceneMgr,
	Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
    OgreBulletDynamics::DynamicsWorld *dyWorld, 
	OIS::Mouse *mouse, OIS::Keyboard *keyboard,
	ARLib::VideoPlayer *videoPlayerLeft, ARLib::VideoPlayer *videoPlayerRight)
	: mRenderTarget(nullptr)
	, mSmallRenderTarget(nullptr)
	, enabledNPRRenderer(false)
	, mWatercolorRenderTarget(nullptr)
	, mSmallWatercolorRenderTarget(nullptr)
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = sceneMgr;
    mDynamicsWorld = dyWorld;

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneMgr->setShadowFarDistance(30);

	// rift node:
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 0); // TODO: set correct rigid body id!
	mRiftNode->getBodyNode()->setPosition(4.0f, 1.5f, 4.0f);
	if (tracker)
		tracker->addRigidBodyEventListener(mRiftNode);

	// create viewports
	if (window && rift)
	{
		mRenderTarget = new ARLib::RiftRenderTarget(rift, root, window);
		mWatercolorRenderTarget = new NPRWatercolorRenderTarget(root, mRenderTarget, 1461, 1182, 1461 / 10, 1182 / 8, 0.1f);
        mRiftNode->addRenderTarget(mRenderTarget /*mWatercolorRenderTarget*/);
	}

	if (smallWindow)
	{
		mSmallRenderTarget = new ARLib::DebugRenderTarget(smallWindow);
		mSmallWatercolorRenderTarget = new NPRWatercolorRenderTarget(root, mSmallRenderTarget, 1461/2, 1182/2, 1461 / 10, 1182 / 8, 0.1f);
        mRiftNode->addRenderTarget(mSmallRenderTarget /*mSmallWatercolorRenderTarget*/);
	}

	// attach video screens
	mRiftVideoScreens = new ARLib::RiftVideoScreens(mRiftNode, videoPlayerLeft, videoPlayerRight);

	// other stuff in the scene
	RigidListenerNode* cubeNodeT = new RigidListenerNode(mSceneMgr->getRootSceneNode(), mSceneMgr, 0);
	if (tracker)
		tracker->addRigidBodyEventListener(cubeNodeT);
	
	Ogre::SceneNode* cubeNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode3->attachObject( cubeEnt3 );
	cubeNode3->setPosition( -1.0, 0.0, 0.0 );
	cubeNode3->setScale( 0.5, 0.5, 0.5 );

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0f, 0.07f, 0.017f );
	roomLight->setSpecularColour( .25f, .25f, .25f );
	roomLight->setDiffuseColour( 0.85f, 0.76f, 0.7f );

	roomLight->setPosition( 5, 5, 5 );

	mSceneMgr->getRootSceneNode()->attachObject( roomLight );

	Ogre::SceneNode* cubeNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition( 3.0f, 0.0f, 0.0f );
	cubeNode2->setScale( 0.5f, 0.5f, 0.5f );

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0f, 0.07f, 0.017f );
	light->setSpecularColour( .25f, .25f, .25f );
	light->setDiffuseColour( 0.35f, 0.27f, 0.23f );
	mRiftNode->getBodyNode()->attachObject(light);

	// Overlay stuff
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	// Create a panel
	Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(
		overlayManager->createOverlayElement("Panel", "TextPanel"));
	panel->setMetricsMode(Ogre::GMM_RELATIVE);
	panel->setPosition(0.0f, 0.2f);
	panel->setDimensions(0.5f, 0.2f);
	// Create a text area
	mTextArea = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager->createOverlayElement("TextArea", "Text"));
	mTextArea->setMetricsMode(Ogre::GMM_RELATIVE);
	mTextArea->setPosition(0, 0);
	mTextArea->setDimensions(0.5f, 0.2f);
	mTextArea->setCaption("Time: ? ms");
	mTextArea->setCharHeight(0.1f);
	mTextArea->setFontName("DefaultFont");
	mTextArea->setColourBottom(Ogre::ColourValue(1.0, 0.0, 0.5));
	mTextArea->setColourTop(Ogre::ColourValue(1.0, 0.0, 0.5));
	Ogre::Overlay* overlay = overlayManager->create("TextOverlay");
	overlay->add2D(panel);
	panel->addChild(mTextArea);
	overlay->show();
	mTextArea->hide(); // hide by default
}

WebcamScene::~WebcamScene()
{
	delete mRiftVideoScreens;
	delete mWatercolorRenderTarget;
	delete mSmallWatercolorRenderTarget;
	delete mRenderTarget;
	delete mSmallRenderTarget;

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

void WebcamScene::toggleNPRRenderer()
{
	mRiftNode->removeAllRenderTargets();

	if (enabledNPRRenderer)
	{
		if (mRenderTarget && mRenderTarget != mSmallRenderTarget)
			mRiftNode->addRenderTarget(mRenderTarget);
		if (mSmallRenderTarget)
			mRiftNode->addRenderTarget(mSmallRenderTarget);
		enabledNPRRenderer = false;
	}
	else
	{
		if (mRenderTarget && mRenderTarget != mSmallRenderTarget)
			mRiftNode->addRenderTarget(mWatercolorRenderTarget);
		if (mSmallRenderTarget)
			mRiftNode->addRenderTarget(mSmallWatercolorRenderTarget);
		enabledNPRRenderer = true;
	}
}

void WebcamScene::update(float dt)
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

	// update video frames
	mRiftVideoScreens->update();

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
	
	// update text box time
	static Ogre::Timer timer;
	if (mTextArea->isVisible())
	{
		char buf[256];
		unsigned long time = timer.getMilliseconds();
		_snprintf(buf, 256, "Time: %lu ms", time);
		mTextArea->setCaption(buf);
	}
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool WebcamScene::keyPressed( const OIS::KeyEvent& e )
{
	if (e.key == OIS::KC_N)
		toggleNPRRenderer();
	if (e.key == OIS::KC_T)
	{
		if (mTextArea->isVisible())
			mTextArea->hide();
		else
			mTextArea->show();
	}

	// TODO: save the configuration in a file?
	// video offsets
	const float offsetStep = 0.005f;
	bool setOffsets = false;
	static Ogre::Vector2 offset[2] = {};
	// left
	if (e.key == OIS::KC_D) { offset[0].x += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_A) { offset[0].x -= offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_W) { offset[0].y += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_S) { offset[0].y -= offsetStep; setOffsets = true; }
	// right
	if (e.key == OIS::KC_L) { offset[1].x += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_J) { offset[1].x -= offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_I) { offset[1].y += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_K) { offset[1].y -= offsetStep; setOffsets = true; }
	// IPD adjustment
	if (e.key == OIS::KC_B) { offset[0].x += offsetStep; offset[1].x -= offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_V) { offset[0].x -= offsetStep; offset[1].x += offsetStep; setOffsets = true; }

	if (setOffsets) mRiftVideoScreens->setOffsets(offset[0], offset[1]);

	// video scalings
	const float scaleStep = 0.005f;
	bool setScalings = false;
	static Ogre::Vector2 scale = Ogre::Vector2(1.0f, 1.0f);

	// same for both for now...?
	if (e.key == OIS::KC_RIGHT) { scale.x -= scaleStep; setScalings = true; }
	if (e.key == OIS::KC_LEFT ) { scale.x += scaleStep; setScalings = true; }
	if (e.key == OIS::KC_UP   ) { scale.y -= scaleStep; setScalings = true; }
	if (e.key == OIS::KC_DOWN ) { scale.y += scaleStep; setScalings = true; }

	if (setScalings) mRiftVideoScreens->setScalings(scale, scale);

	return true;
}
bool WebcamScene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool WebcamScene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mRiftNode->setYaw(Ogre::Degree(-0.3f*e.state.X.rel));
		mRiftNode->setPitch(Ogre::Degree(-0.3f*e.state.Y.rel));
	}
	return true;
}
bool WebcamScene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool WebcamScene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}