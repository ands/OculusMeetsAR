#ifndef APP_H
#define APP_H

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "ARLib/Oculus/Rift.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "ARLib/ARLibOgre.h"
#include "SoundScene.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"


class SoundApp : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
	public:
		SoundApp(bool showDebugWindow);
		~SoundApp();

		void quit();

		bool keyPressed(const OIS::KeyEvent& e);
		bool keyReleased(const OIS::KeyEvent& e);
		bool mouseMoved(const OIS::MouseEvent& e);
		bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		bool update();

	private:
		void initOgre(bool showDebugWindow);
		void quitOgre();
        void initBullet(bool enableDebugDrawing);
        void quitBullet();
		void initOIS();
		void quitOIS();
		void initRift();
		void quitRift();
		void initTracking();
		void quitTracking();
		void createViewports();

		OIS::Keyboard* mKeyboard;
		OIS::Mouse* mMouse;

		Ogre::Root* mRoot;
        Ogre::SceneManager *mSceneMgr;

		Ogre::RenderWindow* mWindow;
		Ogre::RenderWindow* mSmallWindow;
		

		bool mShutdown;

		SoundScene* mScene;

		bool mRiftAvailable;
		bool mTrackingAvailable;
		ARLib::Rift* mRift;
		ARLib::TrackingManager* mTracker;
		ARLib::RenderTarget* mRenderTarget;
		ARLib::RenderTarget* mSmallRenderTarget;

        OgreBulletCollisions::DebugDrawer *mDebugDrawer;
        OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;
        OgreBulletCollisions::CollisionShape *mGroundShape;
};

#endif
