#include "ARLib/Ogre/RiftVideoScreens.h"
#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/VideoTexture.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "ARLib/Tracking/TrackingManager.h"
#include "OGRE/OgreRectangle2D.h"

namespace ARLib {
	RiftVideoScreens::RiftVideoScreens(Ogre::SceneManager *sceneManager, RiftSceneNode *_riftNode, VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight, TrackingManager *_trackingManager)
		: riftNode(_riftNode)
		, trackingManager(_trackingManager)
	{
		VideoPlayer *videoPlayer[] = { videoPlayerLeft, videoPlayerRight };
		Ogre::Camera *camera[] = { riftNode->getLeftCamera(), riftNode->getRightCamera() };

		for (int eyeNum = 0; eyeNum < 2; eyeNum++)
		{
			const char *textureName[] = { "ARLib/Video/LeftTexture", "ARLib/Video/RightTexture" };
			const char *undistortionTextureName[] = { "ARLib/Video/UndistortionTextureLeft", "ARLib/Video/UndistortionTextureRight" };
			videoTexture[eyeNum] = new ARLib::VideoTexture(videoPlayer[eyeNum], textureName[eyeNum], undistortionTextureName[eyeNum]);

			// video background rendering rect
			Ogre::Rectangle2D *rect = new Ogre::Rectangle2D(true);
			Ogre::uint visibilityMask = camera[eyeNum]->getViewport()->getVisibilityMask(); // only visible to one of the cameras
			rect->setVisibilityFlags(visibilityMask);
			rect->setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
			rect->setUVs(Ogre::Vector2(1, 0), Ogre::Vector2(0, 0), Ogre::Vector2(1, 1), Ogre::Vector2(0, 1));
			rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
			rect->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
			const char *materialName[] = { "ARLib/Video/LeftEye", "ARLib/Video/RightEye" };
			rect->setMaterial(materialName[eyeNum]);
			
			materialPass[eyeNum] = rect->getMaterial()->getTechnique(0)->getPass(0);
			materialPass[eyeNum]->getTextureUnitState(0)->setTexture(videoTexture[eyeNum]->getUndistortionMapTexture());
			materialPass[eyeNum]->getTextureUnitState(1)->setTexture(videoTexture[eyeNum]->getTexture());

			const char *nodeName[] = { "ARLib/Video/LeftScreen", "ARLib/Video/RightScreen" };
			if (trackingManager)
			{
				screenNode[eyeNum] = sceneManager->createSceneNode(nodeName[eyeNum]);
				screenNode[eyeNum]->setPosition(riftNode->getBodyNode()->getPosition());
				screenNode[eyeNum]->setOrientation(riftNode->getHeadNode()->getOrientation());
			}
			else
				screenNode[eyeNum] = riftNode->getHeadNode()->createChildSceneNode(nodeName[eyeNum]);
			screenNode[eyeNum]->attachObject(rect);
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
				/*QueryPerformanceCounter(&currentTimeStamp);
				double latency = (double)(currentTimeStamp.QuadPart - captureTimeStamp.QuadPart) / (double)frequency.QuadPart;
				printf("video latency: >%02lfms\n", latency * 1000.0);*/

				// look up rift pose at captureTimeStamp and position the screen there
				if (trackingManager)
				{
					RigidBody *rigidBody = trackingManager->evaluateRift(captureTimeStamp.QuadPart);
					if (rigidBody)
					{
						Ogre::Vector3 position(rigidBody->mX, rigidBody->mY, rigidBody->mZ);
						Ogre::Quaternion orientation(rigidBody->mqW, rigidBody->mqX, rigidBody->mqY, rigidBody->mqZ);

						screenNode[eyeNum]->setPosition(position);
						screenNode[eyeNum]->setOrientation(orientation);
					}
				}
			}
		}
	}
}; // ARLib namespace
