#include "NPRWatercolorRenderTarget.h"

NPRWatercolorRenderTarget::NPRWatercolorRenderTarget(
	Ogre::Root *root, ARLib::RenderTarget *destination,
	Ogre::uint eyeTextureWidth, Ogre::uint eyeTextureHeight)
	: root(root)
	, watercolorSceneManager(NULL)
{
	// get all the managers we will need
	watercolorSceneManager = root->createSceneManager(Ogre::ST_GENERIC);
	Ogre::TextureManager *textureManager = &Ogre::TextureManager::getSingleton();
	Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
	Ogre::SceneNode *meshNode = watercolorSceneManager->getRootSceneNode()->createChildSceneNode();

	// initialize both eye render targets
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		// create render target textures
		const char *renderTextureNames[] = { "WatercolorRenderTextureLeft", "WatercolorRenderTextureRight" };
		renderTexture[eyeNum] = textureManager->createManual(renderTextureNames[eyeNum],
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
			eyeTextureWidth, eyeTextureHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

		// load materials and set shader parameters
		const char *materialNames[] = { "Watercolor/LeftEye", "Watercolor/RightEye" };
		material[eyeNum] = materialManager->getByName(materialNames[eyeNum]);
		Ogre::Pass *materialPass = material[eyeNum]->getTechnique(0)->getPass(0);
		materialPass->getTextureUnitState(0)->setTexture(renderTexture[eyeNum]);
		
		// create the distortion meshes:
		const char *objectNames[] = { "WatercolorVoronoiLeft", "WatercolorVoronoiRight" };
		Ogre::ManualObject* manual = watercolorSceneManager->createManualObject(objectNames[eyeNum]);
		manual->begin(materialNames[eyeNum], Ogre::RenderOperation::OT_TRIANGLE_LIST);

		int index = 0;
		const int triangles = 15;
		const int countX = 32;
		const int countY = 32;
		const float randRadius = 0.5f / std::max(countX, countY);
		const float coneRadius = (1.0f / std::min(countX, countY) + 2.0f * randRadius) * 1.42f;
		assert(coneRadius > randRadius);

		for (int y = 0; y < countY; y++)
		{
			for (int x = 0; x < countX; x++)
			{
				float texX = ((float)x / (float)(countX - 1)) + ((float)std::rand()/(float)RAND_MAX) * randRadius;
				float texY = ((float)y / (float)(countY - 1)) + ((float)std::rand()/(float)RAND_MAX) * randRadius;
				float centerX = texX * 2.0f - 1.0f;
				float centerY = texY * 2.0f - 1.0f;
				float currX, currY, lastX = centerX + coneRadius, lastY = centerY;

				for(int i = 1; i <= triangles; i++)
				{
					float angle = ((float)i / (float)triangles) * 2.0f * 3.14159f;
					currX = centerX + std::cosf(angle) * coneRadius;
					currY = centerY + std::sinf(angle) * coneRadius;

					// center
					manual->position(centerX, centerY, 0);
					manual->textureCoord(texX, -texY);
					manual->index(index++);

					// first
					manual->position(lastX, lastY, -0.1f);
					manual->textureCoord(texX, -texY);
					manual->index(index++);

					// second
					manual->position(currX, currY, -0.1f);
					manual->textureCoord(texX, -texY);
					manual->index(index++);

					lastX = currX;
					lastY = currY;
				}
			}
		}
 
		manual->end();
		meshNode->attachObject(manual);

		// create eye camera in the watercolor scene so the mesh can be rendered onto it:
		const char *cameraNames[] = { "WatercolorCameraLeft", "WatercolorCameraRight" };
		camera[eyeNum] = watercolorSceneManager->createCamera(cameraNames[eyeNum]);
		camera[eyeNum]->setFarClipDistance(2);
		camera[eyeNum]->setNearClipDistance(0.1f);
		camera[eyeNum]->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		camera[eyeNum]->setOrthoWindow(2, 2);

		watercolorSceneManager->getRootSceneNode()->attachObject(camera[eyeNum]);
	}

	meshNode->setPosition(0, 0, -1);

	destination->SetCameras(camera[0], camera[1]);
}

NPRWatercolorRenderTarget::~NPRWatercolorRenderTarget()
{
	if (watercolorSceneManager)
		root->destroySceneManager(watercolorSceneManager);
}

void NPRWatercolorRenderTarget::SetCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	Ogre::Camera *cameras[2] = { left, right };

	// attach both cameras
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		Ogre::RenderTexture* renderTextureTarget = renderTexture[eyeNum]->getBuffer()->getRenderTarget();
		renderTextureTarget->removeAllViewports();
		renderTextureTarget->addViewport(cameras[eyeNum]);
		renderTextureTarget->getViewport(0)->setClearEveryFrame(true);
		renderTextureTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
		renderTextureTarget->getViewport(0)->setOverlaysEnabled(true);
		// TODO: add blur compositor to camera!
	}
}