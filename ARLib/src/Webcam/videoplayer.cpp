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
			int count1 = fscanf(file, "%lf %lf %lf\n", &homographyMatrix[0], &homographyMatrix[1], &homographyMatrix[2]);
			int count2 = fscanf(file, "%lf %lf %lf\n", &homographyMatrix[3], &homographyMatrix[4], &homographyMatrix[5]);
			int count3 = fscanf(file, "%lf %lf %lf\n", &homographyMatrix[6], &homographyMatrix[7], &homographyMatrix[8]);
			fclose(file);

			if (count1 ==3 && count2 == 3 && count3 == 3)
				homographyLoaded = true;
			else
				fprintf(stderr, "ERROR: Could not read homography matrix from %s\n", homographyMatrixFilename);
		}
		else
			fprintf(stderr, "ERROR: Could not open %s\n", homographyMatrixFilename);
	}

	if (!homographyLoaded)
	{
		double identity[] =
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
	float *test;

	// ocam undistortion
	if (ocamModel)
	{
		ocam_create_perspecive_undistortion_map(ocamModel, xyMap, width, height, videoDistance);
	}
	else
	{
		// calculates the identity xy map
		float *localXYmap = xyMap;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				*localXYmap++ = x;
				*localXYmap++ = y;
			}
		}
	}


	//create double array copy of xyMap
	float *localXYm = xyMap;
	double **xMapTemp = new double*[height];
	for(int i = 0; i < height; ++i) {
		xMapTemp[i] = new double[width];
	}
	double **yMapTemp = new double*[height];
	for(int i = 0; i < height; ++i) {
		yMapTemp[i] = new double[width];
	}
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			xMapTemp[y][width-1-x]=(double)(width-1.0)-*localXYm++;
			yMapTemp[y][x]=*localXYm++;
		}
	}

	// homography "undistortion"
	double **xMapTempHom = new double*[height];
	for(int i = 0; i < height; ++i) {
		xMapTempHom[i] = new double[width];
	}
	double **yMapTempHom = new double*[height];
	for(int i = 0; i < height; ++i) {
		yMapTempHom[i] = new double[width];
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			double inverseZ =    1.0 / ((double)(y) * homographyMatrix[6] + (double)(width-1.0-x) * homographyMatrix[7] + homographyMatrix[8]);
			yMapTempHom[y][x] = inverseZ * ((double)(y) * homographyMatrix[0] + (double)(width-1.0-x) * homographyMatrix[1] + homographyMatrix[2]);
			xMapTempHom[y][x] = (double)(width-1.0)-inverseZ * ((double)(y) * homographyMatrix[3] + (double)(width-1.0-x) * homographyMatrix[4] + homographyMatrix[5]);
		}
	}

	
	float *localXYm2=xyMap;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//TODO: bilinear interpolation?
			int ycoord = (int)(yMapTempHom[y][x]+0.5);
			int xcoord = (int)(xMapTempHom[y][x]+0.5);
			if(0<=ycoord&&ycoord<height && 0<=xcoord&&xcoord<width){
				*localXYm2++ = xMapTemp[ycoord][xcoord];
				*localXYm2++ = yMapTemp[ycoord][xcoord];
			}
			else{
				*localXYm2++=0;
				*localXYm2++=0;
			}
		}
	}

	//Normalization
	float *normXYmap = xyMap;
	float invMaxX = 1.0f / (float)(width - 1);
	float invMaxY = 1.0f / (float)(height - 1);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float mapX = normXYmap[0], mapY = normXYmap[1];
			*normXYmap++ =mapX*invMaxX;
			*normXYmap++ =mapY*invMaxY;
		}
	}
}

}; // ARLib namespace
