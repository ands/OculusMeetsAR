#ifndef ARLIB_VIDEOPLAYER_WEBCAMSTATE_H
#define ARLIB_VIDEOPLAYER_WEBCAMSTATE_H

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

	//Gets the current webcam stream sample and copies it to mTexture
	HRESULT update();

	//capture class
	CCapture *cap;

	//Texture to copy the streamsamples to
	Ogre::TexturePtr mTexture;

	// 0 for left and 1 for right camera
	int camNumber;
};

}

#endif

