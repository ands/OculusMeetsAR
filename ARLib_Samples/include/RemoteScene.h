#ifndef REMOTESCENE_H
#define REMOTESCENE_H

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
	class CompositorInstance;
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
class GlowRenderTarget;
class StarWarsRemotePuppet;
class StarWarsRemote;
class StarWarsLightSaber;
class RigidListenerNode;

class RemoteScene
{
	public:
		RemoteScene(ARLib::Rift *rift, ARLib::TrackingManager *tracker, Ogre::Root *root, Ogre::RenderWindow* window, Ogre::RenderWindow* smallWindow,
            Ogre::SceneManager *sceneMgr, OgreBulletDynamics::DynamicsWorld *dyWorld, OIS::Keyboard *keyboard,
			ARLib::VideoPlayer *leftVideoPlayer, ARLib::VideoPlayer *rightVideoPlayer);
		~RemoteScene();

        void toggleGlow();

		void update(float dt);

		bool keyPressed(const OIS::KeyEvent&);
		bool keyReleased(const OIS::KeyEvent&);
	private:
		void toggleNPRRenderer();
		void setAdditionalLatency(double seconds);

        OgreBulletDynamics::DynamicsWorld *mDynamicsWorld;

        bool mToggle;
        StarWarsRemotePuppet *mRemotePuppet;
        StarWarsRemote* mRemote;
        StarWarsLightSaber* mSword;

		RigidListenerNode *mSwordParentNode;

		Ogre::Root* mRoot;
		OIS::Keyboard* mKeyboard;
		Ogre::SceneManager* mSceneMgr;
		ARLib::RiftSceneNode* mRiftNode;

        Ogre::CompositorInstance *mGlow[2];

		NPRWatercolorRenderTarget* mWatercolorRenderTarget;
		NPRWatercolorRenderTarget* mSmallWatercolorRenderTarget;
		ARLib::VideoPlayer* mVideoPlayerLeft;
		ARLib::VideoPlayer* mVideoPlayerRight;
		ARLib::RiftVideoScreens* mRiftVideoScreens;
		bool enabledNPRRenderer;
		double additionalLatency;

		Ogre::Vector2 mVideoOffset[2];
		Ogre::Vector2 mVideoScale[2];

        ARLib::RenderTarget* mRenderTarget;
        ARLib::RenderTarget* mSmallRenderTarget;
		GlowRenderTarget *mGlowRenderTarget;
		GlowRenderTarget *mSmallGlowRenderTarget;
};

#endif
