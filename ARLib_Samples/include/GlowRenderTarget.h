#ifndef GLOWRENDERTARGET_H
#define GLOWRENDERTARGET_H

#include "OGRE/Ogre.h"
#include "ARLib/Ogre/RenderTarget.h"

class GlowRenderTarget
	: public ARLib::RenderTarget
{
	public:
		GlowRenderTarget(ARLib::RenderTarget *_destination);
		~GlowRenderTarget();

		// sets the cameras that should render to this render target
		virtual void setCameras(
			Ogre::Camera *left,
			Ogre::Camera *right);

	private:
		ARLib::RenderTarget *destination;
		Ogre::Camera *camera[2];
		Ogre::CompositorInstance *glow;
};

#endif
