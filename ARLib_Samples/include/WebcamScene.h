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

namespace OIS
{
	class Keyboard;
	class KeyEvent;
};

class NPRWatercolorRenderTarget;
class RigidListenerNode;

class WebcamScene
{
	public:
		WebcamScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker,
			Ogre::Root *root, Ogre::SceneManager *sceneMgr,
			Ogre::RenderWindow *window, Ogre::RenderWindow *smallWindow,
			OIS::Keyboard *keyboard,
			ARLib::VideoPlayer *videoPlayerLeft, ARLib::VideoPlayer *videoPlayerRight);
		~WebcamScene();

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
		Ogre::Vector2 mVideoScale[2];
};

#endif
