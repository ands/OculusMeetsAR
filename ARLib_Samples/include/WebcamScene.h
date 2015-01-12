#ifndef SCENE_H
#define SCENE_H

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "ARLib/ARLibOgre.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsBoxShape.h"

namespace webcam
{
	class VideoPlayer;
}

class WebcamScene
{
	public:
		WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::Root *root,
			Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, OIS::Mouse *mouse, OIS::Keyboard *keyboard, webcam::VideoPlayer *mVideoPlayerLeft, webcam::VideoPlayer *mVideoPlayerRight);
		~WebcamScene();

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
		ARLib::RiftSceneNode* getRiftSceneNode() { return mRiftNode; }

		void update(float dt);

		// Keyboard and mouse events:
		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
		bool mouseMoved(const OIS::MouseEvent&);
		bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
		bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

	private:
        OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;

        std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
        std::deque<OgreBulletDynamics::RigidBody *> mRigidBodies;

		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		Ogre::SceneNode* mRoomNode;
		ARLib::RiftSceneNode* mRiftNode;
};

#endif
