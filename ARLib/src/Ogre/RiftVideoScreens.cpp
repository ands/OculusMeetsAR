#include "ARLib/Ogre/RiftVideoScreens.h"
#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/VideoTexture.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OGRE/OgreMeshManager.h"

namespace ARLib {
	RiftVideoScreens::RiftVideoScreens(Ogre::SceneManager *sceneManager, RiftSceneNode *_riftNode, VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight, TrackingManager *_trackingManager)
		: riftNode(_riftNode)
		, trackingManager(_trackingManager)
	{
		VideoPlayer *videoPlayer[] = { videoPlayerLeft, videoPlayerRight };
		Ogre::Camera *camera[] = { riftNode->getLeftCamera(), riftNode->getRightCamera() };

		Ogre::MaterialManager *materialManager = &Ogre::MaterialManager::getSingleton();

		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			const char *textureName[] = { "ARLib/Video/LeftTexture", "ARLib/Video/RightTexture" };
			const char *undistortionTextureName[] = { "ARLib/Video/UndistortionTextureLeft", "ARLib/Video/UndistortionTextureRight" };
			videoTexture[eyeNum] = new ARLib::VideoTexture(videoPlayer[eyeNum], textureName[eyeNum], undistortionTextureName[eyeNum]);

			// video rendering plane
			float distance = 0.5f * (camera[eyeNum]->getNearClipDistance() + camera[eyeNum]->getFarClipDistance()); // centered between near and far
			float height = 1.30f * 2.0f * std::tanf(camera[eyeNum]->getFOVy().valueRadians()) * distance;
			float width = height * camera[eyeNum]->getAspectRatio();
			Ogre::MovablePlane plane(Ogre::Vector3::UNIT_Z, -distance);
			const char *meshName[] = { "ARLib/Video/LeftScreenMesh", "ARLib/Video/RightScreenMesh" };
			Ogre::MeshPtr planeMesh = Ogre::MeshManager::getSingleton().createPlane(
				meshName[eyeNum], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				plane, width, height,
				1, 1, false, 1, 1, 1, Ogre::Vector3::UNIT_Y);
		
			const char *entityName[] = { "ARLib/Video/LeftScreenEntity", "ARLib/Video/RightScreenEntity" };
			Ogre::Entity *planeEntity = sceneManager->createEntity(entityName[eyeNum], planeMesh);
			Ogre::uint visibilityMask = camera[eyeNum]->getViewport()->getVisibilityMask(); // only visible to one of the cameras
			planeEntity->setVisibilityFlags(visibilityMask);
			planeEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
			const char *materialName[] = { "ARLib/Video/LeftEye", "ARLib/Video/RightEye" };
			Ogre::MaterialPtr material = materialManager->getByName(materialName[eyeNum]);
			planeEntity->setMaterial(material);
			
			materialPass[eyeNum] = material->getTechnique(0)->getPass(0);
			materialPass[eyeNum]->getTextureUnitState(0)->setTexture(videoTexture[eyeNum]->getUndistortionMapTexture());
			materialPass[eyeNum]->getTextureUnitState(1)->setTexture(videoTexture[eyeNum]->getTexture());
			//Ogre::GpuProgramParametersSharedPtr parameters = materialPass[eyeNum]->getFragmentProgramParameters();
			//parameters->setNamedConstant("coordinateMap", 0);
			//parameters->setNamedConstant("videoTexture", 1);

			const char *nodeName[] = { "ARLib/Video/LeftScreen", "ARLib/Video/RightScreen" };
			screenNode[eyeNum] = riftNode->getHeadCalibrationNode()->createChildSceneNode(nodeName[eyeNum]);
			screenNode[eyeNum]->attachObject(planeEntity);
		}

		setOffsets(Ogre::Vector2::ZERO, Ogre::Vector2::ZERO);
		setScalings(Ogre::Vector2::UNIT_SCALE, Ogre::Vector2::UNIT_SCALE);
	}

	RiftVideoScreens::~RiftVideoScreens()
	{
		screenNode[0]->getParent()->removeChild(screenNode[0]);
		screenNode[1]->getParent()->removeChild(screenNode[1]);
		delete videoTexture[0];
		delete videoTexture[1];
	}

	void RiftVideoScreens::setOffsets(Ogre::Vector2 leftOffset, Ogre::Vector2 rightOffset)
	{
		materialPass[0]->getVertexProgramParameters()->setNamedConstant("offset", Ogre::Vector2(leftOffset.y - 0.5f, leftOffset.x - 0.5f));
		materialPass[1]->getVertexProgramParameters()->setNamedConstant("offset", Ogre::Vector2(rightOffset.y - 0.5f, rightOffset.x - 0.5f));
	}

	void RiftVideoScreens::setScalings(Ogre::Vector2 leftScale, Ogre::Vector2 rightScale)
	{
		materialPass[0]->getVertexProgramParameters()->setNamedConstant("scale", Ogre::Vector2(leftScale.y, leftScale.x));
		materialPass[1]->getVertexProgramParameters()->setNamedConstant("scale", Ogre::Vector2(rightScale.y, rightScale.x));
	}

	void RiftVideoScreens::update()
	{
		LARGE_INTEGER captureTimeStamp;//, currentTimeStamp, frequency;
		//QueryPerformanceFrequency(&frequency);

		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			if (videoTexture[eyeNum]->update(&captureTimeStamp))
			{
				//QueryPerformanceCounter(&currentTimeStamp);
				//double latency = (double)(currentTimeStamp.QuadPart - captureTimeStamp.QuadPart) / (double)frequency.QuadPart;
				//printf("video latency: >%02lfms\n", latency * 1000.0);
				//printf("latency: %I64d\n", currentTimeStamp.QuadPart - captureTimeStamp.QuadPart);

				// look up rift pose at captureTimeStamp and position the screen there
				if (trackingManager)
				{
					RigidBody *rigidBody = trackingManager->evaluateRigidBody(riftNode->getRigidBodyID(), captureTimeStamp.QuadPart);
					if (rigidBody)
					{
						screenNode[eyeNum]->setPosition(rigidBody->mX, rigidBody->mY, rigidBody->mZ);
						screenNode[eyeNum]->setOrientation(rigidBody->mqW, rigidBody->mqX, rigidBody->mqY, rigidBody->mqZ);
					}
				}
			}
		}
	}
}; // ARLib namespace
