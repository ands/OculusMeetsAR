#include "OGRE/OgreMeshManager.h"
#include "OGRE/OgreMaterialManager.h"
#include "OGRE/OgreCamera.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreEntity.h"
#include "ARLib/Ogre/RiftVideoScreens.h"
#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/VideoTexture.h"
#include "ARLib/Video/VideoPlayer.h"
#include "ARLib/Tracking/TrackingManager.h"

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
			// create video texture
			const char *textureName[] = { "ARLib/Video/LeftTexture", "ARLib/Video/RightTexture" };
			const char *undistortionTextureName[] = { "ARLib/Video/UndistortionTextureLeft", "ARLib/Video/UndistortionTextureRight" };
			videoTexture[eyeNum] = new ARLib::VideoTexture(videoPlayer[eyeNum], textureName[eyeNum], undistortionTextureName[eyeNum]);

			// video rendering plane
			float distance = 0.5f * (camera[eyeNum]->getNearClipDistance() + camera[eyeNum]->getFarClipDistance()); // centered between near and far
			screenHeight = 1.30f * 2.0f * std::tanf(camera[eyeNum]->getFOVy().valueRadians()) * distance;
			screenWidth = screenHeight * camera[eyeNum]->getAspectRatio();
			Ogre::MovablePlane plane(Ogre::Vector3::UNIT_Z, -distance);
			const char *meshName[] = { "ARLib/Video/LeftScreenMesh", "ARLib/Video/RightScreenMesh" };
			Ogre::MeshPtr planeMesh = Ogre::MeshManager::getSingleton().createPlane(
				meshName[eyeNum], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				plane, screenWidth, screenHeight,
				1, 1, false, 1, 1, 1, Ogre::Vector3::UNIT_Y);
		
			// video plane entity
			const char *entityName[] = { "ARLib/Video/LeftScreenEntity", "ARLib/Video/RightScreenEntity" };
			Ogre::Entity *planeEntity = sceneManager->createEntity(entityName[eyeNum], planeMesh);
			Ogre::uint visibilityMask = camera[eyeNum]->getViewport()->getVisibilityMask(); // only visible to one of the cameras
			planeEntity->setVisibilityFlags(visibilityMask);
			planeEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
			const char *materialName[] = { "ARLib/Video/LeftEye", "ARLib/Video/RightEye" };
			Ogre::MaterialPtr material = materialManager->getByName(materialName[eyeNum]);
			planeEntity->setMaterial(material);
			
			// assign input textures
			Ogre::Pass *materialPass = material->getTechnique(0)->getPass(0);
			materialPass->getTextureUnitState(0)->setTexture(videoTexture[eyeNum]->getUndistortionMapTexture());
			materialPass->getTextureUnitState(1)->setTexture(videoTexture[eyeNum]->getTexture());

			// insert into scene graph
			const char *screenCalibrationNodeName[] = { "ARLib/Video/LeftScreenCalibration", "ARLib/Video/RightScreenCalibration" };
			const char *screenNodeName[] = { "ARLib/Video/LeftScreen", "ARLib/Video/RightScreen" };
			screenNode[eyeNum] = riftNode->getHeadCalibrationNode()->createChildSceneNode(screenNodeName[eyeNum]);
			screenCalibrationNode[eyeNum] = screenNode[eyeNum]->createChildSceneNode(screenCalibrationNodeName[eyeNum]);
			screenCalibrationNode[eyeNum]->attachObject(planeEntity);
		}
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
		screenCalibrationNode[0]->setPosition(screenWidth * leftOffset.x, screenHeight * leftOffset.y -0.05f, 0.25f);
		screenCalibrationNode[1]->setPosition(screenWidth * rightOffset.x, screenHeight * rightOffset.y -0.05f, 0.25f);
	}

	void RiftVideoScreens::setScalings(Ogre::Vector2 leftScale, Ogre::Vector2 rightScale)
	{
		screenCalibrationNode[0]->setScale(leftScale.x, leftScale.y, 1.0f);
		screenCalibrationNode[1]->setScale(rightScale.x, rightScale.y, 1.0f);
	}

	void RiftVideoScreens::update()
	{
		LARGE_INTEGER captureTimeStamp;

		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			if (videoTexture[eyeNum]->update(&captureTimeStamp))
			{
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
