#include <stdio.h>
#include "ARLib/Webcam/CCapture.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "ARLib/Webcam/ocam.h"

namespace ARLib {

VideoPlayer::VideoPlayer(int cameraNumber, const char *ocamModelParametersFilename, float _videoDistance, const char *homographyMatrixFilename)
	: videoDistance(_videoDistance)
	, ocamModel(NULL)
{
	if (FAILED(CCapture::CreateInstance(&capture)))
		capture = NULL;

	if (capture)
	{
		// camera selection
		IMFActivate *temp = NULL;
		DeviceList list = DeviceList();
		list.EnumerateDevices();
		int count = list.Count();
		// todo: select the two C310 in right order
		bool firstfound = false;
		for(unsigned int i = 0; i < list.Count(); i++)
		{
			temp = list.m_ppDevices[i];
			WCHAR *name = NULL;
			temp->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &name, NULL);
			const WCHAR *searchname = L"Logitech HD Webcam C310";
			// Accepts "Logitech HD Webcam Cx" with x>=310
			if (!wcsncmp(name, searchname, 20) && wcsncmp(name, searchname, sizeof(searchname)) >= 0) {
				if(cameraNumber == 0)
					break;
				else if(!firstfound)
					firstfound = true;
				else
					break;
			}
		}
		capture->StartCapture(temp);

		if (ocamModelParametersFilename)
			ocamModel = ocam_get_model(ocamModelParametersFilename);
	}

	// load homography matrix from file
	bool homographyLoaded = false;
	if (homographyMatrixFilename)
	{
		FILE *file = fopen(homographyMatrixFilename, "r");
		if(file)
		{
			int count1 = fscanf(file, "%f %f %f", &homographyMatrix[0], &homographyMatrix[1], &homographyMatrix[2]);
			int count2 = fscanf(file, "%f %f %f", &homographyMatrix[3], &homographyMatrix[4], &homographyMatrix[5]);
			int count3 = fscanf(file, "%f %f %f", &homographyMatrix[6], &homographyMatrix[7], &homographyMatrix[8]);
			fclose(file);

			if (count1 == count2 == count3 == 3)
				homographyLoaded = true;
			else
				fprintf(stderr, "ERROR: Could not read homography matrix from %s\n", homographyMatrixFilename);
		}
		else
			fprintf(stderr, "ERROR: Could not open %s\n", homographyMatrixFilename);
	}

	if (!homographyLoaded)
	{
		float identity[] =
		{
			1, 0, 0, 
			0, 1, 0, 
			0, 0, 1
		};
		memcpy(homographyMatrix, identity, sizeof(identity));
	}
}

VideoPlayer::~VideoPlayer()
{
	capture->EndCaptureSession();
	capture->Release();
	delete ocamModel;
}

void * VideoPlayer::update()
{
	HRESULT check = E_FAIL;
	if(capture && capture->somebufferexist)
	{
		BYTE* sample = capture->getLastImagesample(&check);
		if(SUCCEEDED(check))
			return sample;
	}
	return NULL;
}

int VideoPlayer::getVideoWidth()
{
	return 1280; // TODO: request and read out actual dimensions 
}

int VideoPlayer::getVideoHeight()
{
	return 960; // TODO: request and read out actual dimensions 
}

void VideoPlayer::calculateUndistortionMap(float *xyMap)
{
	int width = getVideoWidth();
	int height = getVideoHeight();

	// ocam undistortion
	if (ocamModel)
	{
		ocam_create_perspecive_undistortion_map(ocamModel, xyMap, width, height, videoDistance);
	}
	else
	{
		// calculates the identity xy map
		float invMaxX = 1.0f / (float)(width - 1);
		float invMaxY = 1.0f / (float)(height - 1);
		float *localXYmap = xyMap;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				*localXYmap++ = x * invMaxX;
				*localXYmap++ = y * invMaxY;
			}
		}
	}

	// homography "undistortion"
	float *localXYmap = xyMap;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float mapX = localXYmap[0], mapY = localXYmap[1];
			float inverseZ =    1.0f / (mapX * homographyMatrix[6] + mapY * homographyMatrix[7] + homographyMatrix[8]);
			*localXYmap++ = inverseZ * (mapX * homographyMatrix[0] + mapY * homographyMatrix[1] + homographyMatrix[2]);
			*localXYmap++ = inverseZ * (mapX * homographyMatrix[3] + mapY * homographyMatrix[4] + homographyMatrix[5]);
		}
	}
}

}; // ARLib namespace
