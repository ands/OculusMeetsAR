#ifndef ARLIB_VIDEOPLAYER_H
#define ARLIB_VIDEOPLAYER_H

#include <string>
#include <memory>

namespace webcam
{

    struct webcamstate;

    /**
     *  Plays a video on an Ogre texture.
     */
    class VideoPlayer
    {
    public:
        VideoPlayer(int camNum);
        ~VideoPlayer();

        /// Start the Webcamstream
        void playVideo ();

        /// This should be called every frame by the user to update the video texture.
        void update();

        /// Stop the stream
        void close();

        /// Return the texture name of the currently playing stream
        std::string getTextureName();
        /// Return the width of the currently playing stream
        int getVideoWidth();
        /// Return the height of the currently playing stream
        int getVideoHeight();
		/// 0 for left and 1 for right camera
		int camNumber;


    private:
        webcamstate* mState;
    };

}

#endif
