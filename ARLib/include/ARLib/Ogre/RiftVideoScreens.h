#ifndef ARLIB_RIFTVIDEOSCREENS_H
#define ARLIB_RIFTVIDEOSCREENS_H

namespace ARLib {

class RiftSceneNode;
class VideoPlayer;
class VideoTexture;

class RiftVideoScreens
{
public:
    RiftVideoScreens(RiftSceneNode *riftNode, VideoPlayer *videoPlayerLeft, VideoPlayer *videoPlayerRight);
	~RiftVideoScreens();

	void update();

private:
	VideoTexture *videoTexture[2];
};

}; // ARLib namespace

#endif
