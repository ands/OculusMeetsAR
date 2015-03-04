#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreSubEntity.h"
#include "OIS/OISKeyboard.h"
#include "ARLib/ARLibOgre.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/Video/VideoPlayer.h"
#include "WebcamScene.h"
#include "NPRWatercolorRenderTarget.h"
#include "RigidListenerNode.h"

WebcamScene::WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::SceneManager *sceneMgr,
	Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
    OIS::Keyboard *keyboard,
	ARLib::VideoPlayer *videoPlayerLeft, ARLib::VideoPlayer *videoPlayerRight)
	: mRoot(root)
	, mKeyboard(keyboard)
	, mSceneMgr(sceneMgr)
	, mRenderTarget(nullptr)
	, mSmallRenderTarget(nullptr)
	, enabledNPRRenderer(false)
	, mWatercolorRenderTarget(nullptr)
	, mSmallWatercolorRenderTarget(nullptr)
	, mVideoPlayerLeft(videoPlayerLeft), mVideoPlayerRight(videoPlayerRight)
	, mRiftVideoScreens(nullptr)
	, additionalLatency(0.048)
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneMgr->setShadowFarDistance(30);

	// rift node:
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 0); // TODO: set correct rigid body id!
	if (tracker)
		tracker->addRigidBodyEventListener(mRiftNode);

	// create viewports
	if (window && rift)
	{
		mRenderTarget = new ARLib::RiftRenderTarget(rift, root, window);
		mWatercolorRenderTarget = new NPRWatercolorRenderTarget(root, mRenderTarget, 1461, 1182, 1461 / 10, 1182 / 8, 0.1f);
        mRiftNode->addRenderTarget(mRenderTarget);
	}

	if (smallWindow)
	{
		mSmallRenderTarget = new ARLib::DebugRenderTarget(smallWindow);
		mSmallWatercolorRenderTarget = new NPRWatercolorRenderTarget(root, mSmallRenderTarget, 1461/2, 1182/2, 1461 / 10, 1182 / 8, 0.1f);
        mRiftNode->addRenderTarget(mSmallRenderTarget);
	}

	// attach video screens
	mRiftVideoScreens = new ARLib::RiftVideoScreens(mSceneMgr, mRiftNode, videoPlayerLeft, videoPlayerRight, tracker);

	// other stuff in the scene
	RigidListenerNode* cubeNodeT = new RigidListenerNode(mSceneMgr->getRootSceneNode(), mSceneMgr, 0);
	if (tracker)
		tracker->addRigidBodyEventListener(cubeNodeT);
	
	Ogre::SceneNode* cubeNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
	cubeNode3->attachObject( cubeEnt3 );
	cubeNode3->setPosition(-0.4f, 0.2f, -0.2f);
	cubeNode3->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows(true);
	roomLight->setShadowFarDistance(30.0f);
	roomLight->setAttenuation(65.0f, 1.0f, 0.07f, 0.017f);
	roomLight->setSpecularColour(0.25f, 0.25f, 0.25f);
	roomLight->setDiffuseColour(0.85f, 0.76f, 0.7f);

	roomLight->setPosition(5.0f, 5.0f, 5.0f);

	mSceneMgr->getRootSceneNode()->attachObject( roomLight );

	Ogre::SceneNode* cubeNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
	cubeNode2->attachObject( cubeEnt2 );
	cubeNode2->setPosition(0.6f, 0.0f, 0.0f);
	cubeNode2->setScale(0.1f, 0.1f, 0.1f);

	Ogre::SceneNode* cubeNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* cubeEnt1 = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt1->getSubEntity(0)->setMaterialName( "CubeMaterialRed" );
	cubeNode1->attachObject( cubeEnt1 );
	cubeNode1->setPosition(0.4f, -0.2f, 0.6f);
	cubeNode1->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows(false);
	light->setAttenuation(65.0f, 1.0f, 0.07f, 0.017f);
	light->setSpecularColour(0.25f, 0.25f, 0.25f);
	light->setDiffuseColour(0.35f, 0.27f, 0.23f);
	mRiftNode->getBodyNode()->attachObject(light);

	mVideoOffset[0] = Ogre::Vector2(0.060f, -0.016f);
	mVideoOffset[1] = Ogre::Vector2(0.004f, -0.016f);
	mVideoScale[0] = Ogre::Vector2(1.02f, 1.11f);
	mVideoScale[1] = Ogre::Vector2(1.00f, 1.11f);
	mRiftVideoScreens->setOffsets(mVideoOffset[0], mVideoOffset[1]);
	mRiftVideoScreens->setScalings(mVideoScale[0], mVideoScale[1]);

	setAdditionalLatency(additionalLatency);
}

WebcamScene::~WebcamScene()
{
	delete mRiftVideoScreens;
	delete mWatercolorRenderTarget;
	delete mSmallWatercolorRenderTarget;
	delete mRenderTarget;
	delete mSmallRenderTarget;
	mRoot->destroySceneManager(mSceneMgr);
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
	if (rift && mKeyboard->isKeyDown(OIS::KC_R))
		rift->recenterPose();

	mRiftVideoScreens->update();
}

void WebcamScene::setAdditionalLatency(double seconds)
{
	LARGE_INTEGER frequency, additionalLatency;
	QueryPerformanceFrequency(&frequency);
	additionalLatency.QuadPart = (LONGLONG)(seconds * frequency.QuadPart);
	mVideoPlayerLeft->setAdditionalLatency(additionalLatency);
	mVideoPlayerRight->setAdditionalLatency(additionalLatency);
	printf("additional latency: %03lfs\n", seconds);
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool WebcamScene::keyPressed( const OIS::KeyEvent& e )
{
	if (e.key == OIS::KC_N)
		toggleNPRRenderer();

	// video offsets
	const float offsetStep = 0.004f;
	bool setOffsets = false;
	// left
	if (e.key == OIS::KC_D) { mVideoOffset[0].x -= offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_A) { mVideoOffset[0].x += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_W) { mVideoOffset[0].y += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_S) { mVideoOffset[0].y -= offsetStep; setOffsets = true; }
	// right
	if (e.key == OIS::KC_L) { mVideoOffset[1].x -= offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_J) { mVideoOffset[1].x += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_I) { mVideoOffset[1].y += offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_K) { mVideoOffset[1].y -= offsetStep; setOffsets = true; }
	// IPD adjustment
	if (e.key == OIS::KC_M) { mVideoOffset[0].x += 0.5f * offsetStep; mVideoOffset[1].x -= 0.5f * offsetStep; setOffsets = true; }
	if (e.key == OIS::KC_B) { mVideoOffset[0].x -= 0.5f * offsetStep; mVideoOffset[1].x += 0.5f * offsetStep; setOffsets = true; }

	if (setOffsets)
	{
		mRiftVideoScreens->setOffsets(mVideoOffset[0], mVideoOffset[1]);
		printf("offset L: %02f x %02f\tR: %02f x %02f\n", mVideoOffset[0].x, mVideoOffset[0].y, mVideoOffset[1].x, mVideoOffset[1].y);
	}

	// video scalings
	const float scaleStep = 0.01f;
	bool setScalings = false;
	// same for both for now...?
	if (e.key == OIS::KC_RIGHT) { mVideoScale[0].x -= scaleStep; setScalings = true; }
	if (e.key == OIS::KC_LEFT ) { mVideoScale[0].x += scaleStep; setScalings = true; }
	if (e.key == OIS::KC_UP   ) { mVideoScale[0].y -= scaleStep; setScalings = true; }
	if (e.key == OIS::KC_DOWN ) { mVideoScale[0].y += scaleStep; setScalings = true; }
	mVideoScale[1] = mVideoScale[0]; // TODO: scaling for both!!!

	if (setScalings)
	{
		mRiftVideoScreens->setScalings(mVideoScale[0], mVideoScale[1]);
		printf("scale: %02f x %02f\n", mVideoScale[0].x, mVideoScale[0].y);
	}

	// video latency
	const double latencyStep = 0.002;
	bool setLatency = false;
	if (e.key == OIS::KC_0) { additionalLatency -= latencyStep; setLatency = true; }
	if (e.key == OIS::KC_9) { additionalLatency += latencyStep; setLatency = true; }
	if (setLatency) setAdditionalLatency(additionalLatency);

	return true;
}
bool WebcamScene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}