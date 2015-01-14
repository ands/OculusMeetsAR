#ifndef ARLIB_VIDEOPLAYER_WEBCAMSTATE_H
#define ARLIB_VIDEOPLAYER_WEBCAMSTATE_H

#include <boost/thread.hpp>
#include <OGRE/OgreTexture.h>
#include <ARLib/Webcam/capture.h>

namespace webcam{

struct webcamstate;

/**
*  Plays a video on an Ogre texture.
*/

struct webcamstate
{
	webcamstate(int camNum);
	~webcamstate();

	void init();
	void deinit();

	//Cameraselection/activation and captureloop
	static void capture_loop(webcamstate *self);

	//Gets the current webcam stream sample and copies it to mTexture
	HRESULT update();

	//capture class
	CCapture *cap;

	//Texture to copy the streamsamples to
	Ogre::TexturePtr mTexture;

	//Capture Thread for capture_loop
	boost::thread capture_thread;

	//Current webcam stream sample 
	BYTE* sample;

	// 0 for left and 1 for right camera
	int camNumber;
};

}

#endif

