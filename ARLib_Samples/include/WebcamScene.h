#ifndef SCENE_H
#define SCENE_H

#include "OGRE/Ogre.h"
#include "OGRE/Overlay/OgreTextAreaOverlayElement.h"
#include "OIS/OIS.h"
#include "ARLib/ARLibOgre.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsBoxShape.h"

class NPRWatercolorRenderTarget;

class WebcamScene
{
	public:
		WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
			Ogre::Root *root, Ogre::SceneManager *sceneMgr,
			Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
			OgreBulletDynamics::DynamicsWorld *dyWorld,
			OIS::Mouse *mouse, OIS::Keyboard *keyboard,
			ARLib::VideoPlayer *videoPlayerLeft, ARLib::VideoPlayer *videoPlayerRight);
		~WebcamScene();

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
		ARLib::RiftSceneNode* getRiftSceneNode() { return mRiftNode; }

		void update(float dt);

		void setAdditionalLatency(double seconds);

		// Keyboard and mouse events:
		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
		bool mouseMoved(const OIS::MouseEvent&);
		bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
		bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

	private:
		void toggleNPRRenderer();

        OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;

        std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
        std::deque<OgreBulletDynamics::RigidBody *> mRigidBodies;

		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		Ogre::TextAreaOverlayElement* mTextArea;
		ARLib::RiftSceneNode* mRiftNode;
		ARLib::RenderTarget* mRenderTarget;
		ARLib::RenderTarget* mSmallRenderTarget;
		NPRWatercolorRenderTarget* mWatercolorRenderTarget;
		NPRWatercolorRenderTarget* mSmallWatercolorRenderTarget;
		ARLib::VideoPlayer* mVideoPlayerLeft;
		ARLib::VideoPlayer* mVideoPlayerRight;
		ARLib::RiftVideoScreens* mRiftVideoScreens;
		bool enabledNPRRenderer;
		double additionalLatency;

		Ogre::Vector2 mVideoOffset[2];
		Ogre::Vector2 mVideoScale;
};

#endif
