#ifndef ARLIB_VIDEOPLAYER_H
#define ARLIB_VIDEOPLAYER_H

namespace ARLib {

class CCapture;
struct ocam_model;

class VideoPlayer
{
public:
    VideoPlayer(
		int cameraNumber = 0,
		const char *ocamModelParametersFilename = NULL, float videoDistance = 4.0f,
		const char *homographyMatrixFilename = NULL);
    ~VideoPlayer();
	// Returns the width of the video image
	int getVideoWidth();
	// Returns the height of the video image
	int getVideoHeight();
	// Returns true if camera exists
	bool cameraExists();
	// Sets a value that is substracted from each captureTimeStamp
	void setAdditionalLatency(LARGE_INTEGER _additionalLatency) { additionalLatency = _additionalLatency; }
	// Returns a pointer to the current video frame memory in the BGR format
	// or NULL if there is no new frame
	// captureTimeStamp is set to the value of QueryPerformanceCounter() during arrival
	void * beginUpdate(LARGE_INTEGER *captureTimeStamp = NULL);
	void endUpdate();
	// Precalculates a coordinate mapping for undistortion
	void calculateUndistortionMap(float *xyMap);

private:
	CCapture *capture;
	LARGE_INTEGER additionalLatency;
	float videoDistance;
	ocam_model *ocamModel;
	float homographyMatrix[3 * 3];
	bool cameraFound;
};

}; // ARLib namespace

#endif
