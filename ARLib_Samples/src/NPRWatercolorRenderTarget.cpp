#include "OGRE/OgreTextureManager.h"
#include "OGRE/OgreMaterialManager.h"
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreManualObject.h"
#include "OGRE/OgreHardwarePixelBuffer.h"
#include "OGRE/OgreCompositorManager.h"
#include "OGRE/OgreCompositorInstance.h"
#include "OGRE/OgreCompositionPass.h"
#include "OGRE/OgreCompositionTargetPass.h"
#include "NPRWatercolorRenderTarget.h"

NPRWatercolorRenderTarget::NPRWatercolorRenderTarget(
	Ogre::Root *root, ARLib::RenderTarget *destination,
	Ogre::uint eyeTextureWidth, Ogre::uint eyeTextureHeight,
	Ogre::uint tilesX, Ogre::uint tilesY,
	Ogre::Real edgeThreshold)
	: root(root)
	, destination(destination)
{
	watercolorSceneManager[0] = NULL;
	watercolorSceneManager[1] = NULL;

	// get all the managers we will need
	Ogre::TextureManager *textureManager = &Ogre::TextureManager::getSingleton();
	Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();

	// initialize both eye render targets
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		watercolorSceneManager[eyeNum] = root->createSceneManager(Ogre::ST_GENERIC);
		Ogre::SceneNode *meshNode = watercolorSceneManager[eyeNum]->getRootSceneNode()->createChildSceneNode();

		const char *materialNames[] = { "Watercolor/LeftEye", "Watercolor/RightEye" };
		material[eyeNum] = materialManager->getByName(materialNames[eyeNum]);
		Ogre::Pass *materialPass = material[eyeNum]->getTechnique(0)->getPass(0);

		// blurred target
		{
			// create render target textures
			const char *renderTextureNames[] = { "WatercolorRenderTextureBlurredLeft", "WatercolorRenderTextureBlurredRight" };
			renderTextureBlurred[eyeNum] = textureManager->createManual(renderTextureNames[eyeNum],
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
				eyeTextureWidth, eyeTextureHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
			materialPass->getTextureUnitState(0)->setTexture(renderTextureBlurred[eyeNum]);
		}

		// original target
		{
			// create render target textures
			const char *renderTextureNames[] = { "WatercolorRenderTextureOriginalLeft", "WatercolorRenderTextureOriginalRight" };
			renderTextureOriginal[eyeNum] = textureManager->createManual(renderTextureNames[eyeNum],
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
				eyeTextureWidth, eyeTextureHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
			materialPass->getTextureUnitState(1)->setTexture(renderTextureOriginal[eyeNum]);
		}

		// set edge detection parameters
		Ogre::GpuProgramParametersSharedPtr params = materialPass->getFragmentProgramParameters();
		params->setNamedConstant("PixelSize", Ogre::Vector2(1.0f / eyeTextureWidth, 1.0f / eyeTextureHeight));
		params->setNamedConstant("EdgeThreshold", edgeThreshold);
		
		// create the voronoi mesh:
		const char *objectNames[] = { "WatercolorVoronoiLeft", "WatercolorVoronoiRight" };
		Ogre::ManualObject* manual = watercolorSceneManager[eyeNum]->createManualObject(objectNames[eyeNum]);

		const int triangles = 15;
		const float randRadiusX = 0.5f / tilesX;
		const float randRadiusY = 0.5f / tilesY;
		const float coneRadiusX = (0.5f / tilesX + randRadiusX) * 1.42f;
		const float coneRadiusY = (0.5f / tilesY + randRadiusY) * 1.42f;
		assert(coneRadiusX > randRadiusX && coneRadiusY > randRadiusY);

		manual->begin(materialNames[eyeNum], Ogre::RenderOperation::OT_TRIANGLE_LIST);
		int index = 0;
		for (unsigned int y = 0; y < tilesY; y++)
		{
			for (unsigned int x = 0; x < tilesX; x++)
			{
				float px = ((float)x / (float)(tilesX - 1)) + (((float)std::rand()/(float)RAND_MAX) * 2.0f - 1.0f) * randRadiusX;
				float py = ((float)y / (float)(tilesY - 1)) + (((float)std::rand()/(float)RAND_MAX) * 2.0f - 1.0f) * randRadiusY;

				float texCenterX = px;
				float texCenterY = 1.0f - py;
				float centerX = px - 0.5f;
				float centerY = py - 0.5f;
				float currX, currY, lastX = centerX + coneRadiusX, lastY = centerY;
				float currTexX, currTexY, lastTexX = texCenterX + coneRadiusX, lastTexY = texCenterY;
				for(int i = 1; i <= triangles; i++)
				{
					float angle = ((float)i / (float)triangles) * 2.0f * 3.14159f;
					float dx = std::cos(angle) * coneRadiusX;
					float dy = std::sin(angle) * coneRadiusY;
					currX = centerX + dx;
					currY = centerY + dy;
					currTexX = texCenterX + dx;
					currTexY = texCenterY - dy;

					// center
					manual->position(centerX, centerY, 0);
					manual->textureCoord(texCenterX, texCenterY);
					manual->textureCoord(texCenterX, texCenterY);
					manual->index(index++);

					// first
					manual->position(lastX, lastY, -0.1f);
					manual->textureCoord(texCenterX, texCenterY);
					manual->textureCoord(lastTexX, lastTexY);
					manual->index(index++);

					// second
					manual->position(currX, currY, -0.1f);
					manual->textureCoord(texCenterX, texCenterY);
					manual->textureCoord(currTexX, currTexY);
					manual->index(index++);

					lastX = currX;
					lastY = currY;
					lastTexX = currTexX;
					lastTexY = currTexY;
				}
			}
		}
 
		manual->end();
		meshNode->attachObject(manual);
		meshNode->setPosition(0, 0, -1);

		// create eye camera in the watercolor scene so the mesh can be rendered onto it:
		const char *cameraNames[] = { "WatercolorCameraLeft", "WatercolorCameraRight" };
		camera[eyeNum] = watercolorSceneManager[eyeNum]->createCamera(cameraNames[eyeNum]);
		camera[eyeNum]->setFarClipDistance(2);
		camera[eyeNum]->setNearClipDistance(0.1f);
		camera[eyeNum]->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		camera[eyeNum]->setOrthoWindow(1, 1);

		watercolorSceneManager[eyeNum]->getRootSceneNode()->attachObject(camera[eyeNum]);


		// create MRT
		const char *mrtNames[] = { "WatercolorMRTLeft", "WatercolorMRTRight" };
		mrt[eyeNum] = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget(mrtNames[eyeNum]);
		renderTextureBlurred[eyeNum]->getBuffer()->getRenderTarget()->setAutoUpdated(false);
		renderTextureOriginal[eyeNum]->getBuffer()->getRenderTarget()->setAutoUpdated(false);
		mrt[eyeNum]->bindSurface(0, renderTextureBlurred[eyeNum]->getBuffer()->getRenderTarget());
		mrt[eyeNum]->bindSurface(1, renderTextureOriginal[eyeNum]->getBuffer()->getRenderTarget());
		mrt[eyeNum]->setAutoUpdated(true);
	}
}

NPRWatercolorRenderTarget::~NPRWatercolorRenderTarget()
{
	if (watercolorSceneManager[0])
		root->destroySceneManager(watercolorSceneManager[0]);
	if (watercolorSceneManager[1])
		root->destroySceneManager(watercolorSceneManager[1]);
}

void NPRWatercolorRenderTarget::setCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	Ogre::Camera *cameras[2] = { left, right };

	// attach both cameras
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		mrt[eyeNum]->removeAllViewports();
		Ogre::Viewport *vp = mrt[eyeNum]->addViewport(cameras[eyeNum]);
		vp->setMaterialScheme("MRT");

		// add blur compositor
		Ogre::CompositorInstance *blur = Ogre::CompositorManager::getSingleton().addCompositor(vp, "Blur");
		blur->setEnabled(true);
		
		// set compositor parameters
		Ogre::Pass *passes[2] = {
			blur->getTechnique()->getTargetPass(1)->getPass(0)->getMaterial().get()->getBestTechnique()->getPass(0),
			blur->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial().get()->getBestTechnique()->getPass(0)
		};
		for (int passIndex = 0; passIndex < 2; passIndex++)
		{
			Ogre::GpuProgramParametersSharedPtr params = passes[passIndex]->getFragmentProgramParameters();
			params->setNamedConstant("BlurRadius", Ogre::Vector2(1.0f / (float)vp->getActualWidth(), 1.0f / (float)vp->getActualHeight()));
		}
	}

	destination->setCameras(camera[0], camera[1]);
}