#include "App.h"

App::App(bool showDebugWindow)
	: mRoot(NULL)
	, mKeyboard(NULL)
	, mMouse(NULL)
	, mScene(NULL)
	, mShutdown(false)
	, mWindow(NULL)
	, mSmallWindow(NULL)
	, mRiftAvailable(false)
	, mRift(NULL)
{
	std::cout << "Creating Ogre application:" << std::endl;

	// check if Oculus Rift (ID 0) is available:
	ARLib::Rift::init();
	mRiftAvailable = ARLib::Rift::available(0);
	if (!mRiftAvailable) // at least show the debug window if it is not available
		showDebugWindow = true;

	initOgre(showDebugWindow);
	initOIS();
	mScene = new Scene(mRoot, mMouse, mKeyboard);
	createViewports();
	initRift();
	mRoot->startRendering();
}

App::~App()
{
	std::cout << "Deleting Ogre application." << std::endl;
	quitRift();
	std::cout << "Deleting Scene:" << std::endl;
	if(mScene) delete mScene;
	std::cout << "Closing OIS:" << std::endl;
	quitOIS();
	std::cout << "Closing Ogre:" << std::endl;
	quitOgre();
}

void App::initOgre(bool showDebugWindow)
{
	Ogre::ConfigFile cf;
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
	cf.load("resources_d.cfg");
#else
	mRoot = new Ogre::Root("plugins.cfg");
	cf.load("resources.cfg");
#endif
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
}
void App::quitOgre()
{
	//if(mRoot) delete mRoot;
}

void App::initOIS()
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
void App::quitOIS()
{
	if(mKeyboard) delete mKeyboard;
	if(mMouse) delete mMouse;
}

void App::initRift()
{
	// try to initialize the Oculus Rift (ID 0):
	if (mRiftAvailable)
	{
		try {
			mRift = new ARLib::Rift(0, mRoot, mWindow);
			ARLib::RiftNode* riftNode = mScene->getRiftNode();
			mRift->setCameras(riftNode->getLeftCamera(), riftNode->getRightCamera());
			riftNode->setIPD(mRift->getIPD());
		} catch(const char* e) {
			std::cout << ">> " << e << std::endl;
			mRiftAvailable = false;
			mRift = NULL;
		}
	}
}
void App::quitRift()
{
	std::cout << "Shutting down Oculus Rifts:" << std::endl;
	if(mRift) delete mRift;
	ARLib::Rift::shutdown();
}

void App::createViewports()
{
	if(mSmallWindow)
	{
		ARLib::RiftNode* riftNode = mScene->getRiftNode();

		Ogre::Viewport* debugL = mSmallWindow->addViewport(riftNode->getLeftCamera(), 0, 0.0f, 0.0f, 0.5f, 1.0f);
		debugL->setBackgroundColour(Ogre::ColourValue(0.15f, 0.15f, 0.15f));

		Ogre::Viewport* debugR = mSmallWindow->addViewport(riftNode->getRightCamera(), 1, 0.5f, 0.0f, 0.5f, 1.0f);
		debugR->setBackgroundColour(Ogre::ColourValue(0.15f, 0.15f, 0.15f));
	}
}

bool App::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
	if (mShutdown) return false;

	if (mRift)
	{
		if (mRift->update(evt.timeSinceLastFrame))
		{
			mScene->getRiftNode()->setPose(mRift->getOrientation(), mRift->getPosition());
		} else {
			delete mRift;
			mRift = NULL;
		}
	}

	// update the standard input devices
	mKeyboard->capture();
	mMouse->capture();

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
bool App::mouseMoved(const OIS::MouseEvent& e)
{
	mScene->mouseMoved(e);
	return true;
}
bool App::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mScene->mouseReleased(e, id);
	return true;
}
bool App::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mScene->mouseReleased(e, id);
	return true;
}

void App::quit()
{
	std::cout << "QUIT." << std::endl;
	mShutdown = true;
}
