
// Main Application for Ogre integration.
// Responsible for comminication with Ogre (rendering) and OIS (mouse and keyboard input)

#ifndef APP_H
#define APP_H

class Rift;

#include "Rift.h"
#include <sstream>
#include <string.h>
#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "Scene.h"
#include "Globals.h"

// The Debug window's size is the Oculus Rift Resolution times this factor.
#define debugWindowSize 0.5

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
	public:
		App();
		~App();

		void initOgre();
		void quitOgre();
		void initOIS();
		void quitOIS();
		void createViewports();

		void quit();

		void initRift();
		void quitRift();

		bool keyPressed(const OIS::KeyEvent& e );
		bool keyReleased(const OIS::KeyEvent& e );
		bool mouseMoved(const OIS::MouseEvent& e );
		bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id );
		bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id );

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		bool update();

	private:

		OIS::Keyboard* mKeyboard;
		OIS::Mouse* mMouse;

		Ogre::Root* mRoot;

		Ogre::RenderWindow* mWindow;
		Ogre::RenderWindow* mSmallWindow;
		Ogre::Viewport* mViewportL;
		Ogre::Viewport* mViewportR;

		// If this is set to true, the app will close during the next frame:
		bool mShutdown;

		Scene* mScene;

		Rift* mRift;
};

#endif
