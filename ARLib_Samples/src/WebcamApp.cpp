#include "WebcamApp.h"
#include "NatNetTypes.h"

WebcamApp::WebcamApp(bool showDebugWindow)
	: mRoot(nullptr)
	, mKeyboard(nullptr)
	, mMouse(nullptr)
	, mScene(nullptr)
	, mShutdown(false)
	, mWindow(nullptr)
	, mSmallWindow(nullptr)
	, mRiftAvailable(false)
	, mRift(nullptr)
	, mTracker(nullptr)
    , mDebugDrawer(nullptr)
    , mDynamicsWorld(nullptr)
    , mGroundShape(nullptr)
	, mVideoPlayerLeft(nullptr)
	, mVideoPlayerRight(nullptr)
{
	std::cout << "Creating Ogre application:" << std::endl;
	//showDebugWindow = false; // for testing purposes
	// check if Oculus Rift (ID 0) is available:
	ARLib::Rift::init();
	mRiftAvailable = ARLib::Rift::available(0);
	if (!mRiftAvailable) // at least show the debug window if it is not available
		showDebugWindow = true;

	initOgre(showDebugWindow);
	initBullet(showDebugWindow); //enable debug drawer
	initOIS();
	initRift();
	initTracking();

	mVideoPlayerLeft  = new ARLib::VideoPlayer(0, "../../media/calib_results_CAM1.txt", 3.0f, "../../media/homography_CAM1.txt" );
	mVideoPlayerRight = new ARLib::VideoPlayer(1, "../../media/calib_results_CAM2.txt", 3.0f, "../../media/homography_CAM2.txt" );
    mScene = new WebcamScene(
		mRift, mTracker,
		mRoot, mSceneMgr,
		mWindow, mSmallWindow,
		mDynamicsWorld,
		mMouse, mKeyboard,
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
    std::cout << "Closing Bullet:" << std::endl;
    quitBullet();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

void WebcamApp::initOgre(bool showDebugWindow)
{
	Ogre::ConfigFile cf;
	mRoot = new Ogre::Root("../../media/plugins.cfg");
	cf.load("../../media/resources.cfg");
	mOverlaySystem = new Ogre::OverlaySystem();
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
	Ogre::RenderSystem* pRS = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	Ogre::ConfigOptionMap cfgMap = pRS->getConfigOptions();
	cfgMap["Full Screen"].currentValue = "No";
	cfgMap["VSync"].currentValue = "Yes";
	#ifdef _DEBUG
		cfgMap["FSAA"].currentValue = "0";
	#else
		cfgMap["FSAA"].currentValue = "8";
	#endif
	cfgMap["Video Mode"].currentValue = "1200 x 800";
	for(Ogre::ConfigOptionMap::iterator iter = cfgMap.begin(); iter != cfgMap.end(); iter++)
		pRS->setConfigOption(iter->first, iter->second.currentValue);
	mRoot->setRenderSystem(pRS);
	mRoot->initialise(false, "ARLib Example");

	// create windows:
	if (mRiftAvailable)
	{
		Ogre::NameValuePairList miscParams;
		miscParams["monitorIndex"] = Ogre::StringConverter::toString(1 /*oculusRiftMonitorId*/);
		miscParams["border"] = "none";
		mWindow = mRoot->createRenderWindow("ARLib Example", 1920, 1080, true, &miscParams);
	}
	if (showDebugWindow)
	{
		Ogre::NameValuePairList miscParamsSmall;
		miscParamsSmall["monitorIndex"] = Ogre::StringConverter::toString(0 /*debugMonitorId*/);
		mSmallWindow = mRoot->createRenderWindow("ARLib Example (debug window)", 1920 / 2, 1080 / 2, false, &miscParamsSmall);
		if (!mWindow)
			mWindow = mSmallWindow;
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
}
void WebcamApp::quitOgre()
{
	delete mOverlaySystem;
	delete mRoot;
}

void WebcamApp::initBullet(bool enableDebugDrawing){
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    mDynamicsWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, Ogre::AxisAlignedBox(-10,-10,-10,10,10,10), Ogre::Vector3(0,-2,0));
    mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
    mDebugDrawer->setDrawWireframe(true);

    mDynamicsWorld->setDebugDrawer(mDebugDrawer);
    mDynamicsWorld->setShowDebugShapes(true);
    
    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast<Ogre::SimpleRenderable *>(mDebugDrawer));
    //and other stuff!
}

void WebcamApp::quitBullet(){
    if(mGroundShape != nullptr)
        delete mGroundShape;
    if(mDebugDrawer != nullptr)
        delete mDebugDrawer;
    if(mDynamicsWorld != nullptr)
        delete mDynamicsWorld;
}

void WebcamApp::initOIS()
{
	OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    // tell OIS about the Ogre window
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	// setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
    // tell OIS about the window's dimensions
    unsigned int width, height, depth;
    int top, left;
    mWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	// make sure OIS calls callbacks (keyPressed, mouseMoved etc) of this class:
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
}
void WebcamApp::quitOIS()
{
	if(mKeyboard) delete mKeyboard;
	if(mMouse) delete mMouse;
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
		
void WebcamApp::initTracking()
{
	if(mRiftAvailable)
		mTracker = new ARLib::TrackingManager(ARLib::ARLIB_NATNET | ARLib::ARLIB_RIFT, 1000, mRift);
	else
		mTracker = new ARLib::TrackingManager(ARLib::ARLIB_NATNET ,1000);

	mTracker->setNatNetConnectionType(ConnectionType_Multicast);
	mTracker->setNatNetClientIP(); //local machine
	mTracker->setNatNetServerIP(); //local machine

	ARLib::TRACKING_ERROR_CODE error = mTracker->initialize();
	if(error != ARLib::ARLIB_TRACKING_OK){
		std::cout<<"Failed to Initialize Tracking Manager. ErrorCode:"<<error<<std::endl;
		mTrackingAvailable = false;
		mTracker->uninitialize();
		delete mTracker;
		mTracker = nullptr;
	}else{
		mTrackingAvailable = true;	
	}
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

	// update the standard input devices
	mKeyboard->capture();
	mMouse->capture();

    mDynamicsWorld->stepSimulation(evt.timeSinceLastFrame, 10);
	
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
bool WebcamApp::mouseMoved(const OIS::MouseEvent& e)
{
	mScene->mouseMoved(e);
	return true;
}
bool WebcamApp::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mScene->mouseReleased(e, id);
	return true;
}
bool WebcamApp::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mScene->mouseReleased(e, id);
	return true;
}

void WebcamApp::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
