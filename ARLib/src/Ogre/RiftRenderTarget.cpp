#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/RiftRenderTarget.h"

namespace ARLib {

RiftRenderTarget::RiftRenderTarget(Rift *rift, Ogre::Root *root, Ogre::RenderWindow *renderWindow)
	: rift(rift)
	, root(root)
	, riftSceneManager(NULL)
{
	// get rift parameters for both eyes
	int recommendedTexSize[2][2];
	rift->getTextureSizes(
		recommendedTexSize[0],
		recommendedTexSize[1]);

	float uvScale[2][2], uvOffset[2][2];
	rift->getUVScalesAndOffsets(
		uvScale[0], uvOffset[0],
		uvScale[1], uvOffset[1]);

	DistortionVertex *vertices[2];
	unsigned short *indices[2];
	int vertexNum[2], indexNum[2];
	rift->getDistortionGeometries(
		&vertices[0], &vertexNum[0], &indices[0], &indexNum[0],
		&vertices[1], &vertexNum[1], &indices[1], &indexNum[1]);

	// get all the managers we will need
	riftSceneManager = root->createSceneManager(Ogre::ST_GENERIC);
	Ogre::TextureManager *textureManager = &Ogre::TextureManager::getSingleton();
	Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
	Ogre::SceneNode *meshNode = riftSceneManager->getRootSceneNode()->createChildSceneNode();

	// initialize both eye render targets
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		// create render target textures
		const char *renderTextureNames[] = { "RiftRenderTextureLeft", "RiftRenderTextureRight" };
		renderTexture[eyeNum] = textureManager->createManual(renderTextureNames[eyeNum],
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
			recommendedTexSize[eyeNum][0], recommendedTexSize[eyeNum][1], 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

		// load materials and set shader parameters
		const char *materialNames[] = { "Oculus/LeftEye", "Oculus/RightEye" };
		Ogre::MaterialPtr material = materialManager->getByName(materialNames[eyeNum]);
		Ogre::Pass *materialPass = material->getTechnique(0)->getPass(0);
		materialPass->getTextureUnitState(0)->setTexture(renderTexture[eyeNum]);
		Ogre::GpuProgramParametersSharedPtr params = materialPass->getVertexProgramParameters();
		params->setNamedConstant("eyeToSourceUVScale", Ogre::Vector2(uvScale[eyeNum][0], uvScale[eyeNum][1]));
		params->setNamedConstant("eyeToSourceUVOffset", Ogre::Vector2(uvOffset[eyeNum][0], uvOffset[eyeNum][1]));
		
		// create the distortion meshes:
		const char *objectNames[] = { "RiftRenderObjectLeft", "RiftRenderObjectRight" };
		Ogre::ManualObject* manual = riftSceneManager->createManualObject(objectNames[eyeNum]);
		manual->begin(materialNames[eyeNum], Ogre::RenderOperation::OT_TRIANGLE_LIST);

		for(int i = 0; i < vertexNum[eyeNum]; i++)
		{
			DistortionVertex v = vertices[eyeNum][i];
			manual->position(v.ScreenPosNDC[0], v.ScreenPosNDC[1], 0);
			manual->textureCoord(v.TanEyeAnglesR[0], v.TanEyeAnglesR[1]);
			manual->textureCoord(v.TanEyeAnglesG[0], v.TanEyeAnglesG[1]);
			manual->textureCoord(v.TanEyeAnglesB[0], v.TanEyeAnglesB[1]);
			float vig = std::max(v.VignetteFactor, 0.0f);
			manual->colour(vig, vig, vig, vig);
		}
		for(int i = 0; i < indexNum[eyeNum]; i++)
		{
			manual->index(indices[eyeNum][i]);
		}
 
		manual->end();
		meshNode->attachObject(manual);
	}

	// create a camera in the rift scene so the mesh can be rendered onto it:
	Ogre::Camera *combinedCamera = riftSceneManager->createCamera("OculusRiftExternalCamera");
	combinedCamera->setFarClipDistance(2);
	combinedCamera->setNearClipDistance(0.1f);
	combinedCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	combinedCamera->setOrthoWindow(2, 2);

	riftSceneManager->getRootSceneNode()->attachObject(combinedCamera);

	meshNode->setPosition(0, 0, -1);
	meshNode->setScale(1, 1, -1);

	Ogre::Viewport *viewport = renderWindow->addViewport(combinedCamera);
	viewport->setBackgroundColour(Ogre::ColourValue::Black);
	viewport->setOverlaysEnabled(true);
}

RiftRenderTarget::~RiftRenderTarget()
{
	if (riftSceneManager)
		root->destroySceneManager(riftSceneManager);
}

void RiftRenderTarget::SetCameras(Ogre::Camera *left, Ogre::Camera *right)
{
	Ogre::Camera *cameras[2] = { left, right };

	float aspectRatios[2], projections[2][16];
	rift->getProjections(
		left->getNearClipDistance(), // use existing zNear & zFar values of one of the cameras
		left->getFarClipDistance(), true,
		&aspectRatios[0], projections[0],
		&aspectRatios[1], projections[1]);

	// attach both cameras
	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		Ogre::RenderTexture* renderTextureTarget = renderTexture[eyeNum]->getBuffer()->getRenderTarget();
		renderTextureTarget->removeAllViewports();
		renderTextureTarget->addViewport(cameras[eyeNum]);
		renderTextureTarget->getViewport(0)->setClearEveryFrame(true);
		renderTextureTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
		renderTextureTarget->getViewport(0)->setOverlaysEnabled(true);
	
		cameras[eyeNum]->setAspectRatio(aspectRatios[eyeNum]);
	
		float *p = projections[eyeNum];
		cameras[eyeNum]->setCustomProjectionMatrix(true, Ogre::Matrix4(
			p[ 0], p[ 1], p[ 2], p[ 3],
			p[ 4], p[ 5], p[ 6], p[ 7],
			p[ 8], p[ 9], p[10], p[11],
			p[12], p[13], p[14], p[15]));
	}
}

void RiftRenderTarget::SetRiftSceneNode(RiftSceneNode *riftSceneNode)
{
	SetCameras(riftSceneNode->getLeftCamera(), riftSceneNode->getRightCamera());
}

}; // ARLib namespace