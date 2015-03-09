#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreCompositorInstance.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsWorld.h"
#include "OIS/OISKeyboard.h"
#include "ARLib/ARLibOgre.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/Video/VideoPlayer.h"
#include "RemoteScene.h"
#include "LaserBulletManager.h"
#include "GlowMaterialListener.h"
#include "GlowRenderTarget.h"
#include "NPRWatercolorRenderTarget.h"
#include "RigidListenerNode.h"
#include "StarWarsLightSaber.h"
#include "StarWarsRemote.h"
#include "StarWarsRemotePuppet.h"

RemoteScene::RemoteScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
    Ogre::Root *root, Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
	Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld,
	OIS::Keyboard *keyboard,
	ARLib::VideoPlayer *leftVideoPlayer, ARLib::VideoPlayer *rightVideoPlayer)
    : mRoot(root)
	, mKeyboard(keyboard)
	, mSceneMgr(sceneMgr)
	, mDynamicsWorld(dyWorld)
	, mVideoPlayerLeft(leftVideoPlayer), mVideoPlayerRight(rightVideoPlayer)
	, additionalLatency(0.048)
	, mRiftVideoScreens(nullptr)
	, currentRenderTarget(RenderTargetIndex_Glow)
{
	// rift node
	mRiftNode = new ARLib::RiftSceneNode(rift, mSceneMgr, 0.001f, 50.0f, 1);
	if (tracker)
		tracker->addRigidBodyEventListener(mRiftNode);
    
    // render targets
	for (int i = 0; i < RenderTargetIndex_Count; i++)
	{
		mRenderTargets[i] = nullptr;
		mSmallRenderTargets[i] = nullptr;
	}
    if(window && rift)
	{
        mRenderTargets[RenderTargetIndex_Default] = new ARLib::RiftRenderTarget(rift, root, window);
		mRenderTargets[RenderTargetIndex_Glow] = new GlowRenderTarget(mRenderTargets[RenderTargetIndex_Default]);
		mRenderTargets[RenderTargetIndex_NPR] = new NPRWatercolorRenderTarget(root, mRenderTargets[RenderTargetIndex_Default], 1461, 1182, 1461 / 10, 1182 / 8, 0.1f);
		mRenderTargets[RenderTargetIndex_Glow_NPR] = new GlowRenderTarget(mRenderTargets[RenderTargetIndex_NPR]);
    }
    if(smallWindow)
	{
        mSmallRenderTargets[RenderTargetIndex_Default] = new ARLib::DebugRenderTarget(smallWindow);
		mSmallRenderTargets[RenderTargetIndex_Glow] = new GlowRenderTarget(mSmallRenderTargets[RenderTargetIndex_Default]);
		mSmallRenderTargets[RenderTargetIndex_NPR] = new NPRWatercolorRenderTarget(root, mSmallRenderTargets[RenderTargetIndex_Default], 1461/2, 1182/2, 1461 / 10, 1182 / 8, 0.1f);
		mSmallRenderTargets[RenderTargetIndex_Glow_NPR] = new GlowRenderTarget(mSmallRenderTargets[RenderTargetIndex_NPR]);
    }
	setRenderTargets(currentRenderTarget);

	mRiftVideoScreens = new ARLib::RiftVideoScreens(mSceneMgr, mRiftNode, leftVideoPlayer, rightVideoPlayer, tracker);
	mVideoOffset[0] = Ogre::Vector2(0.060f, -0.016f);
	mVideoOffset[1] = Ogre::Vector2(0.004f, -0.016f);
	mVideoScale[0] = Ogre::Vector2(1.02f, 1.11f);
	mVideoScale[1] = Ogre::Vector2(1.00f, 1.11f);
	mRiftVideoScreens->setOffsets(mVideoOffset[0], mVideoOffset[1]);
	mRiftVideoScreens->setScalings(mVideoScale[0], mVideoScale[1]);
	setAdditionalLatency(additionalLatency);

	// scene
    LaserBulletManager::getSingleton().setDynamicsWorld(dyWorld);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
	mSceneMgr->setShadowFarDistance(30);

	GlowMaterialListener *gml = new GlowMaterialListener();
	Ogre::MaterialManager::getSingleton().addListener(gml);

    // roomLight
	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30.f );
	roomLight->setAttenuation( 65.0f, 1.0f, 0.07f, 0.017f );
	roomLight->setSpecularColour( .25f, .25f, .25f );
	roomLight->setDiffuseColour( 1.0f, 1.0f, 1.0f );
	roomLight->setPosition( 3.f, 0.5f, 1.f );
	mSceneMgr->getRootSceneNode()->attachObject( roomLight );

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65.0f, 1.0f, 0.07f, 0.017f );
	light->setSpecularColour( .4f, .4f, .4f );
	light->setDiffuseColour( 0.7f, 0.54f, 0.46f );
	mRiftNode->getBodyNode()->attachObject(light); 

	Ogre::SceneNode *swordOffsetNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	swordOffsetNode->setPosition(0,0,-0.2f);
    mSwordParentNode = new RigidListenerNode(swordOffsetNode, mSceneMgr, 2);
    mSword = new StarWarsLightSaber(mSwordParentNode->getSceneNode(), mSceneMgr, mDynamicsWorld);
    if(tracker)
        tracker->addRigidBodyEventListener(mSwordParentNode);

	mRemote = new StarWarsRemote(mSceneMgr->getRootSceneNode(), mSceneMgr, mDynamicsWorld, mRiftNode->getHeadNode(),2.0f);
    mRemotePuppet = new StarWarsRemotePuppet(mRemote, mRiftNode->getBodyNode(), mSceneMgr->getRootSceneNode(), mSceneMgr, mDynamicsWorld, 2.0f);
    mRemotePuppet->init(mRiftNode->getHeadNode()->_getDerivedOrientation() * Ogre::Vector3(0,0,-1));
}

RemoteScene::~RemoteScene()
{
	for (int i = 0; i < RenderTargetIndex_Count; i++)
	{
		delete mRenderTargets[i];
		delete mSmallRenderTargets[i];
	}
	mRoot->destroySceneManager(mSceneMgr);
	delete mRiftNode;
}

void RemoteScene::update(float dt)
{
    mRemotePuppet->update(dt);
	mRemote->update(dt);
    mSword->update(dt);
    LaserBulletManager::getSingleton().update(dt);
	
	mRiftVideoScreens->update();
}

void RemoteScene::setRenderTargets(RenderTargetIndex renderTargetIndex)
{
	mRiftNode->removeAllRenderTargets();
	if (mRenderTargets[renderTargetIndex] != mSmallRenderTargets[renderTargetIndex])
		mRiftNode->addRenderTarget(mRenderTargets[renderTargetIndex]);
	if (mSmallRenderTargets[renderTargetIndex])
		mRiftNode->addRenderTarget(mSmallRenderTargets[renderTargetIndex]);
	currentRenderTarget = renderTargetIndex;
	switch(renderTargetIndex)
	{
		case RenderTargetIndex_Default:  printf("current render target: default\n"); break;
		case RenderTargetIndex_Glow:     printf("current render target: Glow\n"); break;
		case RenderTargetIndex_NPR:      printf("current render target: NPR\n"); break;
		case RenderTargetIndex_Glow_NPR: printf("current render target: Glow + NPR\n"); break;
	};
}

void RemoteScene::setAdditionalLatency(double seconds)
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

bool RemoteScene::keyPressed( const OIS::KeyEvent& e )
{
    if(e.key == OIS::KC_C){
		mRiftNode->calibrate();
		mSwordParentNode->calibrate();
		mRemotePuppet->init(mRiftNode->getHeadNode()->_getDerivedOrientation() * Ogre::Vector3(0,0,-1));
    }if(e.key == OIS::KC_V){
        mSword->draw();
    }if(e.key == OIS::KC_N){
        setRenderTargets((RenderTargetIndex)((currentRenderTarget + 1) % RenderTargetIndex_Count));
    }if(e.key == OIS::KC_D){
        mDynamicsWorld->setShowDebugShapes(!mDynamicsWorld->getShowDebugShapes());
    }
	
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
bool RemoteScene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}