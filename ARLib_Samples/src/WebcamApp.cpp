#include "OGRE/OgreRoot.h"
#include "OGRE/OgreConfigFile.h"
#include "OGRE/OgreRenderWindow.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISInputManager.h"
#include "ARLib/Oculus/Rift.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/Video/VideoPlayer.h"
#include "ARLib/ARLibOgre.h"
#include "WebcamScene.h"
#include "WebcamApp.h"
#include "NatNetTypes.h"

WebcamApp::WebcamApp(bool showDebugWindow)
	: mRoot(nullptr)
	, mKeyboard(nullptr)
	, mScene(nullptr)
	, mShutdown(false)
	, mWindow(nullptr)
	, mSmallWindow(nullptr)
	, mRiftAvailable(false)
	, mRift(nullptr)
	, mTracker(nullptr)
	, mVideoPlayerLeft(nullptr)
	, mVideoPlayerRight(nullptr)
{
	std::cout << "Creating Ogre application:" << std::endl;
	showDebugWindow = false; // for testing purposes
	// check if Oculus Rift (ID 0) is available:
	ARLib::Rift::init();
	mRiftAvailable = ARLib::Rift::available(0);
	if (!mRiftAvailable) // at least show the debug window if it is not available
		showDebugWindow = true;

	initOgre(showDebugWindow);
	initOIS();
	initRift();
	initTracking(showDebugWindow);

	Sleep(2000); // needed if the rift tracking camera is connected... too many concurrent usb initializations maybe?

	mVideoPlayerLeft  = new ARLib::VideoPlayer(0, "../../media/calib_results_CAM1.txt", 3.0f, "../../media/homography_CAM1.txt" );
	mVideoPlayerRight = new ARLib::VideoPlayer(1, "../../media/calib_results_CAM2.txt", 3.0f, "../../media/homography_CAM2.txt" );

    mScene = new WebcamScene(
		mRift, mTracker,
		mRoot, mSceneMgr,
		mWindow, mSmallWindow,
		mKeyboard,
		mVideoPlayerLeft, mVideoPlayerRight);
	mRoot->startRendering();
}

WebcamApp::~WebcamApp()
{
	std::cout << "Deleting Ogre application." << std::endl;
	quitTracking();
	quitRift();
	std::cout << "Deleting Scene:" << std::endl;
	if(mScene) delete mScene;
	std::cout << "Closing OIS:" << std::endl;
	quitOIS();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

void WebcamApp::initOgre(bool showDebugWindow)
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
	if (mRiftAvailable)
	{
		Ogre::NameValuePairList miscParams;
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1);
		miscParams["border"] = "none";
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

	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
void WebcamApp::quitOgre()
{
	delete mRoot;
}

void WebcamApp::initOIS()
{
	OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
}
void WebcamApp::quitOIS()
{
	delete mKeyboard;
}

void WebcamApp::initRift()
{
	// try to initialize the Oculus Rift (ID 0):
	if (mRiftAvailable)
	{
		try {
			mRift = new ARLib::Rift(0);
		} catch(const char* e) {
			std::cout << ">> " << e << std::endl;
			mRiftAvailable = false;
			mRift = nullptr;
		}
	}
}
void WebcamApp::quitRift()
{
	std::cout << "Shutting down Oculus Rifts:" << std::endl;
	if(mRift) delete mRift;
	ARLib::Rift::shutdown();
}
		
ARLib::TRACKING_ERROR_CODE WebcamApp::initTracking(ARLib::TRACKING_METHOD method, bool enableDebugLog)
{
	mTracker = new ARLib::TrackingManager(method, 100, enableDebugLog);
	mTracker->setNatNetConnectionType(ConnectionType_Multicast);
	mTracker->setNatNetClientIP("128.176.181.34"); 
	mTracker->setNatNetServerIP("128.176.181.34");
	mTracker->setFrameEvaluationMethod(ARLib::FRAME_FLOOR);

	ARLib::TRACKING_ERROR_CODE error = mTracker->initialize();
	mTrackingAvailable = (error == ARLib::ARLIB_TRACKING_OK);

	if(!mTrackingAvailable)
	{
		mTracker->uninitialize();
		delete mTracker;
		mTracker = nullptr;
	}
	
	return error;
}
		
void WebcamApp::initTracking(bool enableDebugLog)
{
	ARLib::TRACKING_ERROR_CODE error;

	error = initTracking(ARLib::ARLIB_NATNET | ARLib::ARLIB_RIFT, enableDebugLog); // Try both first
	if (error == ARLib::ARLIB_TRACKING_OK)
		std::cout << "NatNet + Rift Tracking initialized." << std::endl;

	if (error == ARLib::ARLIB_TRACKING_NATNET_ERROR)
	{
		error = initTracking(ARLib::ARLIB_RIFT, enableDebugLog); // Rift Tracking only
		if (error == ARLib::ARLIB_TRACKING_OK)
			std::cout << "Rift Tracking initialized." << std::endl;
	}
	else if (error == ARLib::ARLIB_TRACKING_RIFT_ERROR)
	{
		error = initTracking(ARLib::ARLIB_NATNET, enableDebugLog); // NatNet Tracking only
		if (error == ARLib::ARLIB_TRACKING_OK)
			std::cout << "NatNet Tracking initialized." << std::endl;
	}

	if (error != ARLib::ARLIB_TRACKING_OK)
		std::cout << "Failed to Initialize Tracking Manager. ErrorCode:" << error << std::endl;
}
		
void WebcamApp::quitTracking()
{
	std::cout << "Shutting down Tracking System" << std::endl;
	//mTracker->uninitialize(); ::todo
	if(mTracker) delete mTracker;
}

bool WebcamApp::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	if (mShutdown) return false;

	mKeyboard->capture();
	
    if (mTracker)
		mTracker->update(); //right place?
	
    mScene->update(evt.timeSinceLastFrame);

	return true; 
}

bool WebcamApp::keyPressed(const OIS::KeyEvent& e)
{
	mScene->keyPressed(e);

	if(e.key == OIS::KC_ESCAPE)
		quit();
	if(e.key == OIS::KC_P)
		mWindow->writeContentsToFile("Screenshot.png");

	return true;
}
bool WebcamApp::keyReleased(const OIS::KeyEvent& e)
{
	mScene->keyReleased(e);
	return true;
}

void WebcamApp::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
