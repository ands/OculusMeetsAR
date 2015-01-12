#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <string>
#include <memory>

namespace webcam
{

    struct webcamstate;
    //class MovieAudioFactory;

    /**
     * @brief Plays a video on an Ogre texture.
     */
    class VideoPlayer
    {
    public:
        VideoPlayer(int camNum);
        ~VideoPlayer();

        /// @brief Set the MovieAudioFactory to use.
        /// @par This class must be implemented by the user and is responsible for reading the decoded audio data.
        /// @note If you do not set up a MovieAudioFactory, then audio streams will be ignored and the video will be played with no sound.
        /// @note Takes ownership of the passed pointer.
        //void setAudioFactory (MovieAudioFactory* factory);

        /// Return true if a video is currently playing and it has an audio stream.
        //bool hasAudioStream();

        /// Play the given video. If a video is already playing, the old video is closed first.
        /// @note The video will be unpaused by default. Use the pause() and play() methods to control pausing.
        void playVideo (const std::string& resourceName);

        /// This should be called every frame by the user to update the video texture.
        /// @return Returns true if the video is still playing, false if we have reached the end of the video stream.
        bool update();

        /// Stop the currently playing video, if a video is playing.
        void close();

        /// Return the texture name of the currently playing video, or "" if no video is playing.
        std::string getTextureName();
        /// Return the width of the currently playing video, or 0 if no video is playing.
        int getVideoWidth();
        /// Return the height of the currently playing video, or 0 if no video is playing.
        int getVideoHeight();
		int camNumber;


    private:
        webcamstate* mState;
    };

}

#endif
