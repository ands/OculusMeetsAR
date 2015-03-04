#ifndef NPRWATERCOLORRENDERTARGET_H
#define NPRWATERCOLORRENDERTARGET_H

#include "OGRE/OgreTexture.h"
#include "OGRE/OgreMaterial.h"
#include "OGRE/OgreMath.h"
#include "ARLib/Ogre/RenderTarget.h"

namespace Ogre
{
	class Root;
	class Camera;
	class SceneManager;
	class MultiRenderTarget;
};

class NPRWatercolorRenderTarget
	: public ARLib::RenderTarget
{
	public:
		NPRWatercolorRenderTarget(
			Ogre::Root *root, ARLib::RenderTarget *destination,
			Ogre::uint eyeTextureWidth, Ogre::uint eyeTextureHeight,
			Ogre::uint tilesX, Ogre::uint tilesY,
			Ogre::Real edgeThreshold = 0.018f);
		~NPRWatercolorRenderTarget();

		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

	private:
		Ogre::Root *root;
		ARLib::RenderTarget *destination;
		Ogre::SceneManager *watercolorSceneManager[2];
		Ogre::TexturePtr renderTextureBlurred[2];
		Ogre::TexturePtr renderTextureOriginal[2];
		Ogre::MultiRenderTarget *mrt[2];
		Ogre::MaterialPtr material[2];
		Ogre::Camera *camera[2];
};

#endif
