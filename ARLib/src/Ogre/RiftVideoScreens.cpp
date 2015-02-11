#include "ARLib/Ogre/RiftVideoScreens.h"
#include "ARLib/Ogre/RiftSceneNode.h"
#include "ARLib/Ogre/VideoTexture.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "OGRE/OgreRectangle2D.h"

namespace ARLib {
	RiftVideoScreens::RiftVideoScreens(RiftSceneNode *riftNode, VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight)
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

			Ogre::Pass *materialPass = rect->getMaterial()->getTechnique(0)->getPass(0);
			materialPass->getTextureUnitState(0)->setTexture(videoTexture[eyeNum]->getUndistortionMapTexture());
			materialPass->getTextureUnitState(1)->setTexture(videoTexture[eyeNum]->getTexture());

			// TODO: make these configurable!
			const float yOffset = 0.08f, yOffsetRight = -0.02f, xDistance = 0.02f;
			Ogre::Vector2 offset[] = { Ogre::Vector2(-yOffset, xDistance), Ogre::Vector2(-yOffset - yOffsetRight, -xDistance) };
			//Ogre::Vector2 scale[] = { Ogre::Vector2(1080.0f / 1280.0f, 960.0f / 960.0f), Ogre::Vector2(1080.0f / 1280.0f, 960.0f / 960.0f) };
			Ogre::Vector2 scale[] = { Ogre::Vector2(0.8f, 0.8f), Ogre::Vector2(0.8f, 0.8f) };

			materialPass->getVertexProgramParameters()->setNamedConstant("offset", offset[eyeNum]);
			materialPass->getVertexProgramParameters()->setNamedConstant("scale", scale[eyeNum]);

			const char *nodeName[] = { "ARLib/Video/LeftScreen", "ARLib/Video/RightScreen" };
			riftNode->getHeadNode()->createChildSceneNode(nodeName[eyeNum])->attachObject(rect);
		}
	}

	RiftVideoScreens::~RiftVideoScreens()
	{
		// TODO: detach screens?
		delete videoTexture[0];
		delete videoTexture[1];
	}

	void RiftVideoScreens::update()
	{
		videoTexture[0]->update();
		videoTexture[1]->update();
	}
}; // ARLib namespace
