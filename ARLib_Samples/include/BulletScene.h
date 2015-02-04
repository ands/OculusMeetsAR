#ifndef SCENE_H
#define SCENE_H

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "RemotePuppet.h"
#include "Remote.h"
#include "ARLib/ARLibOgre.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"

class BulletScene
{
	public:
		BulletScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::Root *root, Ogre::RenderWindow* window, Ogre::RenderWindow* smallWindow,
            Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, OIS::Mouse *mouse, OIS::Keyboard *keyboard);
		~BulletScene();

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
		ARLib::RiftSceneNode* getRiftSceneNode() { return mRiftNode; }

        void setRenderTarget(ARLib::RenderTarget *renderTarget);
        void toggleGlow();

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

        bool mToggle;
        StarWarsRemotePuppet *mRemotePuppet;
        StarWarsRemote* mRemote;

		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		Ogre::SceneNode* mRoomNode;
		ARLib::RiftSceneNode* mRiftNode;

        ARLib::RenderTarget* mRenderTarget;
        ARLib::RenderTarget* mSmallRenderTarget;
};

#endif
