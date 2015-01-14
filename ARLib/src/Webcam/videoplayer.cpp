#include "ARLib/Webcam/videoplayer.hpp"
#include "ARLib/Webcam/webcamstate.h"

namespace webcam
{

VideoPlayer::VideoPlayer(int camNum)
    : mState(NULL) 
{
	camNumber=camNum;
}

VideoPlayer::~VideoPlayer()
{
    if(mState)
        close();
}

void VideoPlayer::playVideo()
{
    if(mState)
        close();

    try {
        mState = new webcamstate(camNumber);
        mState->init();

        // wait until we have the first picture
        while (mState->mTexture.isNull())
        {
            if (!mState->update())
                break;
        }
    }
    catch(std::exception& e) {
        std::cerr<< "Failed to play video: "<<e.what() <<std::endl;
        close();
    }
}

void VideoPlayer::update ()
{
    mState->update();
}

std::string VideoPlayer::getTextureName()
{
    std::string name;
    if (mState && !mState->mTexture.isNull())
        name = mState->mTexture->getName();
    return name;
}

int VideoPlayer::getVideoWidth()
{
    int width=0;
    if (mState && !mState->mTexture.isNull())
        width = mState->mTexture->getWidth();
    return width;
}

int VideoPlayer::getVideoHeight()
{
    int height=0;
    if (mState && !mState->mTexture.isNull())
        height = mState->mTexture->getHeight();
    return height;
}

void VideoPlayer::close()
{
    if(mState)
    {
        mState->deinit();

        delete mState;
        mState = NULL;
    }
}

}
