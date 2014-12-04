#include "Ogre/RiftNode.h"

namespace ARLib {

RiftNode::RiftNode(Rift *rift, Ogre::Root *root, Ogre::RenderWindow *renderWindow, Ogre::SceneManager *sceneManager, float zNear, float zFar, unsigned int rigidBodyID)
	: RigidBodyEventListener(rigidBodyID)
	, rift(rift)
	, root(root)
	, riftSceneManager(NULL)
{
	Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
	bodyNode = rootNode->createChildSceneNode("BodyNode");
	bodyNode->setFixedYawAxis(true); // don't roll!

	bodyTiltNode = bodyNode->createChildSceneNode();
	headNode = bodyTiltNode->createChildSceneNode("HeadNode");

	cameras[0] = sceneManager->createCamera("LeftCamera");
	cameras[1] = sceneManager->createCamera("RightCamera");

	headNode->attachObject(cameras[0]);
	headNode->attachObject(cameras[1]);

	cameras[0]->setNearClipDistance(zNear);
	cameras[1]->setNearClipDistance(zNear);

	cameras[0]->setFarClipDistance(zFar);
	cameras[1]->setFarClipDistance(zFar);

	float ipd = 0.064f;

	if (rift)
	{
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

		float aspectRatios[2], projections[2][16];
		rift->getProjections(zNear, zFar, true,
			&aspectRatios[0], projections[0],
			&aspectRatios[1], projections[1]);

		riftSceneManager = root->createSceneManager(Ogre::ST_GENERIC);
		//riftSceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
		Ogre::TextureManager *textureManager = &Ogre::TextureManager::getSingleton();
		Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();
		Ogre::SceneNode *meshNode = riftSceneManager->getRootSceneNode()->createChildSceneNode();
		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			const char *renderTextureNames[] = { "RiftRenderTextureLeft", "RiftRenderTextureRight" };
			Ogre::TexturePtr renderTexture = textureManager->createManual(renderTextureNames[eyeNum],
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
				recommendedTexSize[eyeNum][0], recommendedTexSize[eyeNum][1], 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

			// Assign the textures to the eyes used later:
			const char *materialNames[] = { "Oculus/LeftEye", "Oculus/RightEye" };
			Ogre::MaterialPtr material = materialManager->getByName(materialNames[eyeNum]);
			Ogre::Pass *materialPass = material->getTechnique(0)->getPass(0);
			materialPass->getTextureUnitState(0)->setTexture(renderTexture);
			Ogre::GpuProgramParametersSharedPtr params = materialPass->getVertexProgramParameters();
			params->setNamedConstant("eyeToSourceUVScale", Ogre::Vector2(uvScale[eyeNum][0], uvScale[eyeNum][1]));
			params->setNamedConstant("eyeToSourceUVOffset", Ogre::Vector2(uvOffset[eyeNum][0], uvOffset[eyeNum][1]));
		
			// Create the Distortion Meshes:
			// TODO: Destroy the manual objects!!
			const char *objectNames[] = { "RiftRenderObjectLeft", "RiftRenderObjectRight" };
			Ogre::ManualObject* manual = riftSceneManager->createManualObject(objectNames[eyeNum]);
			manual->begin(materialNames[eyeNum], Ogre::RenderOperation::OT_TRIANGLE_LIST);
			//manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

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
 
			// tell Ogre, your definition has finished
			manual->end();
			meshNode->attachObject(manual);

			// attach camera
			Ogre::RenderTexture* renderTextureTarget = renderTexture->getBuffer()->getRenderTarget();
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

		// Create a camera in the (new, external) scene so the mesh can be rendered onto it:
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

		ipd = rift->getInterpupillaryDistance();
	}

	// initialize the interpupillary distance
	cameras[0]->setPosition(-ipd/2.0f, 0.0f, 0.0f);
	cameras[1]->setPosition(ipd/2.0f, 0.0f, 0.0f);
}

RiftNode::~RiftNode()
{
	if (riftSceneManager)
		root->destroySceneManager(riftSceneManager);
}

void RiftNode::setPitch(Ogre::Radian angle)
{
	bodyTiltNode->pitch(angle, Ogre::Node::TS_PARENT);
}

void RiftNode::setYaw(Ogre::Radian angle)
{
	bodyNode->yaw(angle);
}

void RiftNode::onChange(RigidBody *rb)
{
	headNode->setOrientation(rb->mOrientation);
	headNode->setPosition(rb->mPosition);
}

}; // ARLib namespace