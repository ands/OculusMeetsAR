#ifndef NPRWATERCOLORRENDERTARGET_H
#define NPRWATERCOLORRENDERTARGET_H

#include "OGRE/Ogre.h"
#include "ARLib/Ogre/RenderTarget.h"

class NPRWatercolorRenderTarget
	: public ARLib::RenderTarget
{
	public:
		NPRWatercolorRenderTarget(
			Ogre::Root *root, ARLib::RenderTarget *destination,
			Ogre::uint eyeTextureWidth, Ogre::uint eyeTextureHeight);
		~NPRWatercolorRenderTarget();

		// sets the cameras that should render to this render target
		virtual void SetCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

	private:
		Ogre::Root *root;
		Ogre::SceneManager *watercolorSceneManager[2];
		Ogre::TexturePtr renderTextureBlurred[2];
		Ogre::TexturePtr renderTextureOriginal[2];
		Ogre::MultiRenderTarget *mrt[2];
		Ogre::MaterialPtr material[2];
		Ogre::Camera *camera[2];
};

#endif
