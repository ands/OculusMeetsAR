#ifndef APP_H
#define APP_H

#include "OGRE/OgreFrameListener.h"
#include "OIS/OISKeyboard.h"

namespace Ogre
{
	struct FrameEvent;
};

namespace OIS
{
	class KeyEvent;
	class Keyboard;
};

namespace ARLib
{
	enum TRACKING_METHOD;
	enum TRACKING_ERROR_CODE;
	class Rift;
	class TrackingManager;
	class VideoPlayer;
};

class WebcamScene;

class WebcamApp : public Ogre::FrameListener, public OIS::KeyListener
{
	public:
		WebcamApp(bool showDebugWindow);
		~WebcamApp();

		void quit();

		bool keyPressed(const OIS::KeyEvent& e);
		bool keyReleased(const OIS::KeyEvent& e);

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		bool update();

	private:
		void initOgre(bool showDebugWindow);
		void quitOgre();
		void initOIS();
		void quitOIS();
		void initRift();
		void quitRift();
		ARLib::TRACKING_ERROR_CODE initTracking(ARLib::TRACKING_METHOD method, bool enableDebugLog);
		void initTracking(bool enableDebugLog);
		void quitTracking();
		void createViewports();

		bool mShutdown;

		OIS::Keyboard* mKeyboard;
		Ogre::Root* mRoot;
        Ogre::SceneManager *mSceneMgr;
		Ogre::RenderWindow* mWindow;
		Ogre::RenderWindow* mSmallWindow;

		bool mRiftAvailable;
		bool mTrackingAvailable;
		ARLib::Rift* mRift;
		ARLib::TrackingManager* mTracker;
		ARLib::VideoPlayer* mVideoPlayerLeft;
		ARLib::VideoPlayer* mVideoPlayerRight;

		WebcamScene* mScene;
};

#endif
