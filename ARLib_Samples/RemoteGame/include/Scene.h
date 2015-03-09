#ifndef SCENE_H
#define SCENE_H

#include "OGRE/OgreVector2.h"

namespace ARLib
{
	class Rift;
	class TrackingManager;
	class VideoPlayer;
	class RiftVideoScreens;
	class RenderTarget;
	class RiftSceneNode;
};

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
};

namespace OgreBulletDynamics
{
	class DynamicsWorld;
};

namespace OIS
{
	class Keyboard;
	class KeyEvent;
};

class StarWarsRemotePuppet;
class StarWarsRemote;
class StarWarsLightSaber;
class RigidListenerNode;

enum RenderTargetIndex
{
	RenderTargetIndex_Default,
	RenderTargetIndex_Glow,
	RenderTargetIndex_NPR,
	RenderTargetIndex_Glow_NPR,
	RenderTargetIndex_Count
};

class Scene
{
	public:
		Scene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::Root *root, Ogre::RenderWindow* window, Ogre::RenderWindow* smallWindow,
            Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, OIS::Keyboard *keyboard,
			ARLib::VideoPlayer *leftVideoPlayer, ARLib::VideoPlayer *rightVideoPlayer);
		~Scene();

		void update(float dt);

		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
	private:
		void setRenderTargets(RenderTargetIndex renderTargetIndex);
		void setAdditionalLatency(double seconds);

		Ogre::Root* mRoot;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		ARLib::RiftSceneNode* mRiftNode;

		ARLib::VideoPlayer* mVideoPlayerLeft;
		ARLib::VideoPlayer* mVideoPlayerRight;
		Ogre::Vector2 mVideoOffset[2];
		Ogre::Vector2 mVideoScale[2];
		double additionalLatency;
		ARLib::RiftVideoScreens* mRiftVideoScreens;

        ARLib::RenderTarget* mRenderTargets[4];
        ARLib::RenderTarget* mSmallRenderTargets[4];
		RenderTargetIndex currentRenderTarget;

        OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;

        StarWarsRemotePuppet *mRemotePuppet;
        StarWarsRemote* mRemote;
        StarWarsLightSaber* mSword;

		RigidListenerNode *mSwordParentNode;
};

#endif
