#ifndef ARLIB_RIFTVIDEOSCREENS_H
#define ARLIB_RIFTVIDEOSCREENS_H

#include "OGRE/OgreVector2.h"

namespace Ogre {
class Pass;
class SceneNode;
}

namespace ARLib {

class RiftSceneNode;
class TrackingManager;
class VideoPlayer;
class VideoTexture;

class RiftVideoScreens
{
public:
    RiftVideoScreens(
		Ogre::SceneManager *sceneManager, RiftSceneNode *riftNode,
		VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight,
		TrackingManager *trackingManager = NULL);
	~RiftVideoScreens();

	void setOffsets(Ogre::Vector2 leftOffset, Ogre::Vector2 rightOffset);
	void setScalings(Ogre::Vector2 leftScale, Ogre::Vector2 rightScale);

	void update();

private:
	RiftSceneNode *riftNode;
	TrackingManager *trackingManager;
	VideoTexture *videoTexture[2];
	Ogre::SceneNode *screenNode[2];
	Ogre::Pass *materialPass[2];
};

}; // ARLib namespace

#endif
