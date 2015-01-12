#include "videoplayer.hpp"
#include "webcamstate.h"

namespace webcam
{

VideoPlayer::VideoPlayer()
    : mState(NULL)
{

}

VideoPlayer::~VideoPlayer()
{
    if(mState)
        close();
}

void VideoPlayer::playVideo(const std::string &resourceName)
{
    if(mState)
        close();

    try {
        mState = new webcamstate;
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

bool VideoPlayer::update ()
{
    if(mState)
        return mState->update();
    return false;
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
