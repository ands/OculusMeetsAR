#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISInputManager.h"
#include "ARLib/Oculus/Rift.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/Video/VideoPlayer.h"
#include "ARLib/ARLibOgre.h"
#include "Scene.h"
#include "App.h"
#include "NatNetTypes.h"

App::App(bool showDebugWindow)
	: mKeyboard(nullptr)
	, mRoot(nullptr)
	, mSceneMgr(nullptr)
	, mWindow(nullptr)
	, mSmallWindow(nullptr)
	, mShutdown(false)
	, mScene(nullptr)
	, mRift(nullptr)
	, mTracker(nullptr)
	, mVideoPlayerLeft(nullptr), mVideoPlayerRight(nullptr)
    , mDebugDrawer(nullptr)
    , mDynamicsWorld(nullptr)
{
	std::cout << "Creating Ogre application:" << std::endl;

	showDebugWindow = false; // overrides the command line option

	initARLib(showDebugWindow);
	initOgre(showDebugWindow);
    initBullet(showDebugWindow); // enable debug drawer if we also have a debug window
	initOIS();

    mScene = new Scene(mRift, mTracker, mRoot,
		mWindow, mSmallWindow, mSceneMgr, 
		mDynamicsWorld, mKeyboard,
		mVideoPlayerLeft, mVideoPlayerRight);
	mRoot->startRendering();
}

App::~App()
{
	std::cout << "Deleting application." << std::endl;
	delete mScene;
	std::cout << "Closing ARLib:" << std::endl;
	quitARLib();
	std::cout << "Closing OIS:" << std::endl;
	quitOIS();
    std::cout << "Closing Bullet:" << std::endl;
    quitBullet();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

void App::initOgre(bool showDebugWindow)
{
	Ogre::ConfigFile cf;
	mRoot = new Ogre::Root("plugins.cfg");
	cf.load("../../media/resources.cfg");
	mRoot->addFrameListener(this);
 
    // add resources
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey(); Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(i->second, i->first, secName);
	}

	// initialize render system
	mRoot->setRenderSystem(mRoot->getRenderSystemByName("OpenGL Rendering Subsystem"));
	mRoot->initialise(false, "ARLib Example");

	// create windows:
	if (mRift)
	{
		Ogre::NameValuePairList miscParams;
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1);
		miscParams["border"] = "none";
		miscParams["vsync"] = "true";
		mWindow = mRoot->createRenderWindow("ARLib Example", 1920, 1080, true, &miscParams);
	}
	if (showDebugWindow)
	{
		Ogre::NameValuePairList miscParamsSmall;
		miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0);
		mSmallWindow = mRoot->createRenderWindow("ARLib Example (debug window)", 1920 / 2, 1080 / 2, false, &miscParamsSmall);
		if (!mWindow)
			mWindow = mSmallWindow;
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
void App::quitOgre()
{
	delete mRoot;
}

void App::initBullet(bool enableDebugDrawing)
{
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    mDynamicsWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, Ogre::AxisAlignedBox(-10,-10,-10,10,10,10), Ogre::Vector3(0,0,0), true, true, 1000);
   
    if(enableDebugDrawing == true)
	{
        mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
        mDebugDrawer->setDrawWireframe(true);

        mDynamicsWorld->setDebugDrawer(mDebugDrawer);
        mDynamicsWorld->setShowDebugShapes(true);

        Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
        node->attachObject(static_cast<Ogre::SimpleRenderable *>(mDebugDrawer));
    }
}
void App::quitBullet()
{
    delete mDebugDrawer;
    delete mDynamicsWorld;
}

void App::initOIS()
{
    size_t windowHnd = 0;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    std::ostringstream windowHndStr;
	windowHndStr << windowHnd;
	OIS::ParamList pl;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
}
void App::quitOIS()
{
	delete mKeyboard;
}

void App::initARLib(bool& showDebugWindow)
{
	std::cout << "Initializing Oculus Rift" << std::endl;
	{
		ARLib::Rift::init(); // initialize the ovr system
		mRift = new ARLib::Rift();
		ARLib::RIFT_ERROR_CODE error = mRift->initialize(0); // initialize the first connected rift
		switch(error)
		{
		case ARLib::RIFT_OK: std::cout << "Oculus Rift initialized" << std::endl; break;
		case ARLib::RIFT_CONNECTION_ERROR: std::cout << "Error: Connection to Oculus Rift failed" << std::endl; break;
		case ARLib::RIFT_TRACKING_ERROR: std::cout << "Error: Failed to initialize the internal Oculus Rift Tracking system" << std::endl; break;
		}

		if (error != ARLib::RIFT_OK)
		{
			delete mRift;
			mRift = nullptr;
			showDebugWindow = true; // at least show the debug window if the rift is not available
		}
	}

	std::cout << "Initializing Tracking System" << std::endl;
	{
		ARLib::TRACKING_ERROR_CODE error;

		error = initTracking(ARLib::ARLIB_NATNET | ARLib::ARLIB_RIFT, showDebugWindow); // Try both first
		if (error == ARLib::ARLIB_TRACKING_OK)
			std::cout << "NatNet + Rift Tracking initialized." << std::endl;

		if (error == ARLib::ARLIB_TRACKING_NATNET_ERROR)
		{
			error = initTracking(ARLib::ARLIB_RIFT, showDebugWindow); // Rift Tracking only
			if (error == ARLib::ARLIB_TRACKING_OK)
				std::cout << "Rift Tracking initialized." << std::endl;
		}
		else if (error == ARLib::ARLIB_TRACKING_RIFT_ERROR)
		{
			error = initTracking(ARLib::ARLIB_NATNET, showDebugWindow); // NatNet Tracking only
			if (error == ARLib::ARLIB_TRACKING_OK)
				std::cout << "NatNet Tracking initialized." << std::endl;
		}

		if (error != ARLib::ARLIB_TRACKING_OK)
			std::cout << "Failed to Initialize Tracking Manager. ErrorCode:" << error << std::endl;
	}

	Sleep(2000); // needed if the rift tracking camera is connected... too many concurrent usb initializations maybe?
	
	std::cout << "Initializing Video System" << std::endl;
	{
		mVideoPlayerLeft  = new ARLib::VideoPlayer(0, "../../media/calib_results_CAM1.txt", 3.0f, "../../media/homography_CAM1.txt");
		mVideoPlayerRight = new ARLib::VideoPlayer(1, "../../media/calib_results_CAM2.txt", 3.0f, "../../media/homography_CAM2.txt");
	}
}
ARLib::TRACKING_ERROR_CODE App::initTracking(ARLib::TRACKING_METHOD method, bool enableDebugLog)
{
	mTracker = new ARLib::TrackingManager(method, 100, enableDebugLog);
	mTracker->setNatNetConnectionType(ConnectionType_Multicast);
	mTracker->setNatNetClientIP("128.176.181.34"); 
	mTracker->setNatNetServerIP("128.176.181.34");
	mTracker->setFrameEvaluationMethod(ARLib::FRAME_FLOOR);

	ARLib::TRACKING_ERROR_CODE error = mTracker->initialize();
	if(error != ARLib::ARLIB_TRACKING_OK)
	{
		mTracker->uninitialize();
		delete mTracker;
		mTracker = nullptr;
	}
	
	return error;
}
void App::quitARLib()
{
	std::cout << "Shutting down Tracking System" << std::endl;
	if (mTracker)
		mTracker->uninitialize();
	delete mTracker;

	std::cout << "Shutting down Oculus Rift:" << std::endl;
	delete mRift;
	ARLib::Rift::shutdown();

	std::cout << "Shutting down Video:" << std::endl;
	delete mVideoPlayerLeft;
	delete mVideoPlayerRight;
}

bool App::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	if (mShutdown) return false;

	mKeyboard->capture();

    if (mTracker)
		mTracker->update(); //right place?

    mDynamicsWorld->stepSimulation(evt.timeSinceLastFrame, 5);
    mScene->update(evt.timeSinceLastFrame);
	return true; 
}

bool App::keyPressed(const OIS::KeyEvent& e)
{
	mScene->keyPressed(e);

	if(e.key == OIS::KC_ESCAPE)
		quit();
	if(e.key == OIS::KC_P)
		mWindow->writeContentsToFile("Screenshot.png");

	return true;
}
bool App::keyReleased(const OIS::KeyEvent& e)
{
	mScene->keyReleased(e);
	return true;
}

void App::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
