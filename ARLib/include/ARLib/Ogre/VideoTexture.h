#ifndef ARLIB_VIDEOTEXTURE_H
#define ARLIB_VIDEOTEXTURE_H

#include "OGRE/OgreTexture.h"
#include "ARLib/Webcam/VideoPlayer.h"

namespace ARLib {

/**
  *  Plays a video on an Ogre texture.
  */
class VideoTexture
{
public:
	// Opens the video device, creates textures and undistortion mapping
    VideoTexture(const Ogre::String& textureName, const Ogre::String& undistortionMapTextureName, int cameraNumber, const char *ocamModelParametersFilename, float distance);
	// Closes the video device
	~VideoTexture();
	// The video texture
	Ogre::TexturePtr getTexture() { return texture; }
	// The coordinate mapping table for undistortion
	Ogre::TexturePtr getUndistortionMapTexture() { return undistortionMapTexture; }
	// Updates the video texture in case of a new frame
	void update();

private:
	VideoPlayer player;
	Ogre::TexturePtr texture;
	Ogre::TexturePtr undistortionMapTexture;
};

}; // ARLib namespace

#endif
