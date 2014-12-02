#ifndef RIFT_H
#define RIFT_H

#include <iostream>

#include "OVR.h"
#include "OGRE/Ogre.h"

namespace ARLib {

class Rift
{
	public:
		Rift(int id, Ogre::Root* root, Ogre::RenderWindow* renderWindow);
		~Rift();
		
		static void init();
		static void shutdown();
		static bool available(int id);
		
		// Must be called to set up viewports correctly.
		// Create the cameras in your scene, then call this function
		void setCameras(Ogre::Camera* left, Ogre::Camera* right);

		// Update Rift data every frame. This should return true as long as data is read from rift.
		bool update(float dt);

		Ogre::Quaternion getOrientation() { return mOrientation; }
		Ogre::Vector3 getPosition() { return mPosition; }

		// returns interpupillary distance in meters: (Default: 0.064m)
		float getIPD() { return mIPD; }
		
		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }

		// Used to reset head position and orientation to "forward".
		// Call this when user presses 'R', for example.
		void recenterPose();

	private:
		static bool isInitialized;

		ovrHmd hmd;

		Ogre::Quaternion mOrientation;
		Ogre::Vector3 mPosition;
		
		Ogre::TexturePtr mLeftEyeRenderTexture;
		Ogre::TexturePtr mRightEyeRenderTexture;

		Ogre::SceneManager* mSceneMgr;
		Ogre::RenderWindow* mRenderWindow;

		Ogre::Camera* mCamera;
		Ogre::Viewport* mViewport;

		Ogre::MaterialPtr mMatLeft;
		Ogre::MaterialPtr mMatRight;

		float mIPD;
};

}; // ARLib namespace

#endif
