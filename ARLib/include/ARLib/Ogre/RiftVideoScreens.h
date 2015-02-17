#ifndef ARLIB_RIFTVIDEOSCREENS_H
#define ARLIB_RIFTVIDEOSCREENS_H

#include "OGRE/OgreVector2.h"

namespace Ogre {
class Pass;
}

namespace ARLib {

class RiftSceneNode;
class VideoPlayer;
class VideoTexture;

class RiftVideoScreens
{
public:
    RiftVideoScreens(RiftSceneNode *riftNode, VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight);
	~RiftVideoScreens();

	void setOffsets(Ogre::Vector2 leftOffset, Ogre::Vector2 rightOffset);
	void setScalings(Ogre::Vector2 leftScale, Ogre::Vector2 rightScale);

	void update();

private:
	VideoTexture *videoTexture[2];
	Ogre::Pass *materialPass[2];
};

}; // ARLib namespace

#endif
