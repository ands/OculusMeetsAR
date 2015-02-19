/*
	Copyright (C) 2012-2013 Ilija Boshkov (Mind Calamity)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

/* 	
	Based on the SSAO Demo by nullsquared.
	Original Thread: http://ogre3d.org/forums/viewtopic.php?f=11&t=47927
*/

#ifndef _PFXSSAO_H_
#define _PFXSSAO_H_
#include <OGRE\Ogre.h>

class PFXSSAO : public Ogre::CompositorInstance::Listener, public Ogre::SceneManager::Listener
{
public:
	PFXSSAO(Ogre::RenderWindow* wnd, Ogre::Camera* cam)
		: mSceneMgr(0)
		, mCamera(0)
		, mViewport(0)
		, mWindow(0)
		, mCompositor(0)
	{
		mWindow = wnd;
		mCamera = cam;
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		mSceneMgr = mCamera->getSceneManager();
		mViewport = mCamera->getViewport();
		initShadows();
		initSSAO();
	}

	~PFXSSAO()
	{
		mCompositor->removeListener(this);
		Ogre::CompositorManager::getSingleton().removeCompositor(mViewport, "ssao");
		mSceneMgr->removeListener(this);
	}

	void setEnabled(bool state)
	{
		mCompositor->setEnabled(state);
	}

	void toggle()
	{
		mCompositor->setEnabled(!mCompositor->getEnabled());
	}

	void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if (pass_id != 42)
			return;

		Ogre::Vector3 farCorner = mCamera->getViewMatrix(true) * mCamera->getWorldSpaceCorners()[4];
		Ogre::Pass *pass = mat->getBestTechnique()->getPass(0);
		Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
		if (params->_findNamedConstantDefinition("farCorner"))
			params->setNamedConstant("farCorner", farCorner);

		params = pass->getFragmentProgramParameters();
		static const Ogre::Matrix4 CLIP_SPACE_TO_IMAGE_SPACE(
			0.5,    0,    0,  0.5,
			0,   -0.5,    0,  0.5,
			0,      0,    1,    0,
			0,      0,    0,    1);
		if (params->_findNamedConstantDefinition("ptMat"))
			params->setNamedConstant("ptMat", CLIP_SPACE_TO_IMAGE_SPACE * mCamera->getProjectionMatrixWithRSDepth());
		if (params->_findNamedConstantDefinition("far"))
			params->setNamedConstant("far", mCamera->getFarClipDistance());
	}

	void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
	{
		float range = light->getAttenuationRange();
		cam->setNearClipDistance(0.01);
		cam->setFarClipDistance(range);
	}
	void shadowTexturesUpdated(size_t) {}
	void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum*) {}
	void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
	void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
	
protected:
	void initShadows()
	{
		mSceneMgr->setShadowTextureSelfShadow(true);
		mSceneMgr->setShadowTextureCasterMaterial("shadow_caster");
		mSceneMgr->setShadowTextureCount(4);

		mSceneMgr->setShadowTextureSize(256);
		mSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
		mSceneMgr->setShadowCasterRenderBackFaces(false);

		const unsigned numShadowRTTs = mSceneMgr->getShadowTextureCount();
		for (unsigned i = 0; i < numShadowRTTs; ++i)
		{
			Ogre::TexturePtr tex = mSceneMgr->getShadowTexture(i);
			Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
			vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
			vp->setClearEveryFrame(true);
		}
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		mSceneMgr->addListener(this);
	}
	
	void initSSAO()
	{
		mCompositor = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "ssao");
		mCompositor->setEnabled(true);
		mCompositor->addListener(this);
	}
	
	Ogre::SceneManager*			mSceneMgr;
	Ogre::Camera*				mCamera;
	Ogre::Viewport*				mViewport;
	Ogre::RenderWindow*			mWindow;
	Ogre::CompositorInstance*	mCompositor;
};
#endif