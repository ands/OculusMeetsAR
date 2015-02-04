#include "ARLib/Webcam/CCapture.h"
#include "ARLib/Webcam/VideoPlayer.h"
#include "ARLib/Webcam/ocam.h"

namespace ARLib {

VideoPlayer::VideoPlayer(int cameraNumber, const char *ocamModelParametersFilename, float _videoDistance)
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
	if (ocamModel)
	{
		ocam_create_perspecive_undistortion_map(ocamModel, xyMap, getVideoWidth(), getVideoHeight(), videoDistance);
	}
	else
	{
		// calculates the identity xy map
		float invMaxX = 1.0f / (float)(getVideoWidth() - 1);
		float invMaxY = 1.0f / (float)(getVideoHeight() - 1);
		for (int y = 0; y < getVideoHeight(); y++)
		{
			for (int x = 0; x < getVideoWidth(); x++)
			{
				*xyMap++ = x * invMaxX;
				*xyMap++ = y * invMaxY;
			}
		}
	}
}

}; // ARLib namespace
