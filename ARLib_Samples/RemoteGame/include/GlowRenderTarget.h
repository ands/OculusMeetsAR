#ifndef GLOWRENDERTARGET_H
#define GLOWRENDERTARGET_H

#include "ARLib/Ogre/RenderTarget.h"

namespace Ogre
{
	class Camera;
};

class GlowRenderTarget
	: public ARLib::RenderTarget
{
	public:
		GlowRenderTarget(ARLib::RenderTarget *_destination);

		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

	private:
		ARLib::RenderTarget *destination;
		Ogre::Camera *camera[2];
};

#endif
