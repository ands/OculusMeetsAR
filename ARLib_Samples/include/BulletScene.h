#ifndef SCENE_H
#define SCENE_H

#include "Sword.h"
#include "Remote.h"
#include "OIS/OIS.h"
#include "PFXSSAO.h"
#include "OGRE/Ogre.h"
#include "RemotePuppet.h"
#include "ARLib/ARLibOgre.h"
#include "RigidListenerNode.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBullet/Collisions/Shapes/OgreBulletCollisionsStaticPlaneShape.h"

class BulletScene
{
	public:
		BulletScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::Root *root, Ogre::RenderWindow* window, Ogre::RenderWindow* smallWindow,
            Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, OIS::Mouse *mouse, OIS::Keyboard *keyboard,
			ARLib::VideoPlayer *leftVideoPlayer, ARLib::VideoPlayer *rightVideoPlayer);
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
        StarWarsLightSaber* mSword;

		RigidListenerNode *mSwordParentNode;

		Ogre::Root* mRoot;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		ARLib::RiftSceneNode* mRiftNode;
        
        PFXSSAO *mDebugLeftSSAO;
        PFXSSAO *mDebugRightSSAO;
        PFXSSAO *mLeftSSAO;
        PFXSSAO *mRightSSAO;

        Ogre::CompositorInstance *mGlow[2];
		ARLib::RiftVideoScreens *mRiftVideoScreens;

		Ogre::Vector2 mVideoOffset[2];
		Ogre::Vector2 mVideoScale;

        ARLib::RenderTarget* mRenderTarget;
        ARLib::RenderTarget* mSmallRenderTarget;
};

#endif
