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

class NPRWatercolorRenderTarget;
class RigidListenerNode;

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
		void toggleNPRRenderer();
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

		ARLib::RenderTarget* mRenderTarget;
		ARLib::RenderTarget* mSmallRenderTarget;
		NPRWatercolorRenderTarget* mWatercolorRenderTarget;
		NPRWatercolorRenderTarget* mSmallWatercolorRenderTarget;
		bool enabledNPRRenderer;
};

#endif
