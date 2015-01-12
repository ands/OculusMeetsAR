#ifndef VIDEOPLAYER_WEBCAMSTATE_H
#define VIDEOPLAYER_WEBCAMSTATE_H

#include <boost/thread.hpp>
#include <OGRE/OgreTexture.h>
#include <capture.h>

namespace webcam{

struct webcamstate;

struct webcamstate
{
	webcamstate();
	~webcamstate();

	void init();
	void deinit();

	static void capture_loop(webcamstate *self);

	HRESULT update();

	CCapture *cap;

	Ogre::TexturePtr mTexture;

	boost::thread capture_thread;

	BYTE* sample;
};

}

#endif

