#include "WebcamScene.h"
#include "RigidListenerNode.h"
#include "ARLIB/Webcam/videoplayer.hpp"

// eye visibility masks
#define VISIBILITY_FLAG_LEFT  (1 << 0)
#define VISIBILITY_FLAG_RIGHT (1 << 1)

WebcamScene::WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::SceneManager *sceneMgr,
	Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
    OgreBulletDynamics::DynamicsWorld *dyWorld, 
	OIS::Mouse *mouse, OIS::Keyboard *keyboard,
	webcam::VideoPlayer *videoPlayerLeft, webcam::VideoPlayer *videoPlayerRight)
	: mRenderTarget(nullptr)
	, mSmallRenderTarget(nullptr)
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
		tracker->registerRigidBodyEventListener(mRiftNode);

	// create viewports
	if (window && rift)
	{
		mRenderTarget = new ARLib::RiftRenderTarget(rift, root, window);
        mRiftNode->addRenderTarget(mRenderTarget);
		mRiftNode->getLeftCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_LEFT);
		mRiftNode->getRightCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_RIGHT);
	}

	if (smallWindow)
	{
		mSmallRenderTarget = new ARLib::DebugRenderTarget(smallWindow);
        mRiftNode->addRenderTarget(mSmallRenderTarget);
		mRiftNode->getLeftCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_LEFT);
		mRiftNode->getRightCamera()->getViewport()->setVisibilityMask(VISIBILITY_FLAG_RIGHT);
	}

	// start background video players for the eyes
	webcam::VideoPlayer *videoPlayer[] = { videoPlayerLeft, videoPlayerRight };
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		videoPlayer[eyeNum]->playVideo(3.0f); // TODO: video distance needs to be tweakable/calculated
		if (!videoPlayer[eyeNum]->getTextureName().empty())
		{
			// video background rendering rect
			Ogre::Rectangle2D *rect = new Ogre::Rectangle2D(true);
			const Ogre::uint visibilityFlags[] = { VISIBILITY_FLAG_LEFT, VISIBILITY_FLAG_RIGHT };
			rect->setVisibilityFlags(visibilityFlags[eyeNum]);
			rect->setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
			rect->setUVs(Ogre::Vector2(1, 1), Ogre::Vector2(0, 1), Ogre::Vector2(1, 0), Ogre::Vector2(0, 0));
			rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
			rect->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
			const char *materialName[] = { "Video/LeftEye", "Video/RightEye" };
			rect->setMaterial(materialName[eyeNum]);

			Ogre::Pass *materialPass = rect->getMaterial()->getTechnique(0)->getPass(0);
			materialPass->getTextureUnitState(0)->setTextureName(videoPlayer[eyeNum]->getUndistortionMapTextureName());
			materialPass->getTextureUnitState(1)->setTextureName(videoPlayer[eyeNum]->getTextureName());

			const char *nodeName[] = { "LeftVideo", "RightVideo" };
			mRiftNode->getHeadNode()->createChildSceneNode(nodeName[eyeNum])->attachObject(rect);
		}
	}

	RigidListenerNode* cubeNodeT = new RigidListenerNode(mSceneMgr->getRootSceneNode(), mSceneMgr);
	if (tracker)
		tracker->registerRigidBodyEventListener(cubeNodeT);
	
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
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.85, 0.76, 0.7 );

	roomLight->setPosition( 5, 5, 5 );

	mSceneMgr->getRootSceneNode()->attachObject( roomLight );

	Ogre::SceneNode* cubeNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition( 3.0, 0.0, 0.0 );
	cubeNode2->setScale( 0.5, 0.5, 0.5 );

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mRiftNode->getBodyNode()->attachObject(light);
}

WebcamScene::~WebcamScene()
{
	if (mRenderTarget) delete mRenderTarget;
	if (mSmallRenderTarget) delete mSmallRenderTarget;

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

bool WebcamScene::keyPressed( const OIS::KeyEvent& e )
{
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
		mRiftNode->setYaw(Ogre::Degree(-0.3*e.state.X.rel));
		mRiftNode->setPitch(Ogre::Degree(-0.3*e.state.Y.rel));
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