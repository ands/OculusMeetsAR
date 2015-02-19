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
		FILE *file;
		fopen_s(&file, homographyMatrixFilename, "r");
		if(file)
		{
			int count1 = fscanf_s(file, "%f %f %f\n", &homographyMatrix[0], &homographyMatrix[1], &homographyMatrix[2]);
			int count2 = fscanf_s(file, "%f %f %f\n", &homographyMatrix[3], &homographyMatrix[4], &homographyMatrix[5]);
			int count3 = fscanf_s(file, "%f %f %f\n", &homographyMatrix[6], &homographyMatrix[7], &homographyMatrix[8]);
			fclose(file);

			if (count1 == 3 && count2 == 3 && count3 == 3)
				homographyLoaded = true;
			else
				fprintf_s(stderr, "ERROR: Could not read homography matrix from %s\n", homographyMatrixFilename);
		}
		else
			fprintf_s(stderr, "ERROR: Could not open %s\n", homographyMatrixFilename);
	}

	if (!homographyLoaded)
	{
		float identity[] =
		{
			1.0, 0.0, 0.0, 
			0.0, 1.0, 0.0, 
			0.0, 0.0, 1.0
		};
		memcpy(homographyMatrix, identity, sizeof(identity));
	}
}

VideoPlayer::~VideoPlayer()
{
	capture->EndCaptureSession();
	capture->Release();
	free(ocamModel);
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

	float Nxc = height / 2.0f;
	float Nyc = width / 2.0f;
	float Nz  = -width / videoDistance;

	double world[3], cam[2];
	world[2] = Nz;

	// ocam + homography "undistortion" and normalization
	float *localXYmap = xyMap;
	float invMaxX = 1.0f / (float)(width - 1);
	float invMaxY = 1.0f / (float)(height - 1);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// homography undistortion
			float inverseZ =   1.0f / (y * homographyMatrix[6] + x * homographyMatrix[7] + homographyMatrix[8]);
			float ycoord = inverseZ * (y * homographyMatrix[0] + x * homographyMatrix[1] + homographyMatrix[2]);
			float xcoord = inverseZ * (y * homographyMatrix[3] + x * homographyMatrix[4] + homographyMatrix[5]);

			// ocam undistortion
			if (ocamModel)
			{
				world[0] = (ycoord - Nxc);
				world[1] = (xcoord - Nyc);
				ocam_world2cam(ocamModel, world, cam);
			}
			else
			{
				cam[1] = xcoord;
				cam[0] = ycoord;
			}

			if(cam[0] >= 0 && cam[0] < height && cam[1] >= 0 && cam[1] < width)
			{
				*localXYmap++ = (float)cam[1] * invMaxX;
				*localXYmap++ = (float)cam[0] * invMaxY;
			}
			else
			{
				// outside value. should sample the (black) border color
				*localXYmap++ = -1.0f;
				*localXYmap++ = -1.0f;
			}
		}
	}
}

}; // ARLib namespace
