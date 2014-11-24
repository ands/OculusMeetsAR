#ifndef RIFT_H
#define RIFT_H

#include <iostream>

#include "OVR.h"
#include "Kernel/OVR_Math.h"
#include "Ogre.h"
using namespace OVR;

class Rift
{
	public:
		Rift( int ID, Ogre::Root* root, Ogre::RenderWindow* renderWindow, bool rotateView );
		~Rift();
		
		static void init();
		static void shutdown();
		
		// Must be called to set up viewports correctly.
		// Create the cameras in your scene, then call this function
		void setCameras( Ogre::Camera* camLeft, Ogre::Camera* camRight );

		// Update Rift data every frame. This should return true as long as data is read from rift.
		bool update( float dt );

		Ogre::Quaternion getOrientation() { return mOrientation; }
		Ogre::Vector3 getPosition() { return mPosition; }

		// returns interpupillary distance in meters: (Default: 0.064m)
		float getIPD() { return mIPD; }
		
		static bool isInitialized;

		Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }

		// Used to reset head position and orientation to "foreward".
		// Call this when user presses 'R', for example.
		void recenterPose();

	private:

		ovrHmd hmd;
		ovrFrameTiming frameTiming;

		int mRiftID;

		Ogre::Quaternion mOrientation;
		Ogre::Vector3 mPosition;
		
		Ogre::TexturePtr mLeftEyeRenderTexture;
		Ogre::TexturePtr mRightEyeRenderTexture;

		Ogre::SceneManager* mSceneMgr;
		Ogre::RenderWindow* mRenderWindow;

		Ogre::Camera* mCamera;
		Ogre::Viewport* mViewport;
		Ogre::SceneNode* mCamNode;

		Ogre::SceneNode* mRoomNode;
		Ogre::Camera* mCamLeft;
		Ogre::Camera* mCamRight;

		Ogre::SceneNode* mHeadNode;
		Ogre::SceneNode* mBodyNode;
		Ogre::SceneNode* mBodyTiltNode;
		
		Ogre::MaterialPtr mMatLeft;
		Ogre::MaterialPtr mMatRight;

		float mIPD;
};

#endif
