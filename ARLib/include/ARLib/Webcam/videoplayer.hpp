#ifndef ARLIB_VIDEOPLAYER_H
#define ARLIB_VIDEOPLAYER_H

#include <string>
#include <memory>
#include <ARLib/Webcam/capture.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreStringConverter.h>

namespace webcam
{
    /**
     *  Plays a video on an Ogre texture.
     */
    class VideoPlayer
    {
    public:
        VideoPlayer(int camNum);
        ~VideoPlayer();

		/// Start the Webcamstream
		void playVideo ();
		/// Stop the stream
		void close();
		/// Return the texture name of the currently playing stream
		std::string getTextureName();
		/// Return the width of the currently playing stream
		int getVideoWidth();
		/// Return the height of the currently playing stream
		int getVideoHeight();
		//Gets the current webcam stream sample and copies it to mTexture
		HRESULT update();

	private:
		/// 0 for left and 1 for right camera
		int camNumber;
		//capture class
		CCapture *cap;
		//Texture to copy the streamsamples to
		Ogre::TexturePtr mTexture;

    };

}

#endif
