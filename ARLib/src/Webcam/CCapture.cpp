#include <new>
#include <assert.h>
#include <shlwapi.h>
#include "ARLib/Webcam/CCapture.h"
#include <strmif.h>
#include <iostream>

namespace ARLib {

	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

	void DeviceList::Clear()
	{
		for (UINT32 i = 0; i < m_cDevices; i++)
		{
			SafeRelease(&m_ppDevices[i]);
		}
		CoTaskMemFree(m_ppDevices);
		m_ppDevices = NULL;
		m_cDevices = 0;
	}

	HRESULT DeviceList::EnumerateDevices()
	{
		MFStartup(MF_VERSION);

		HRESULT hr = S_OK;
		IMFAttributes *pAttributes = NULL;

		Clear();

		// Initialize an attribute store. We will use this to
		// specify the enumeration parameters.
		hr = MFCreateAttributes(&pAttributes, 1);

		// Ask for source type = video capture devices
		if (SUCCEEDED(hr))
		{
			hr = pAttributes->SetGUID(
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
				);
		}

		// Enumerate devices.
		if (SUCCEEDED(hr))
		{
			hr = MFEnumDeviceSources(pAttributes, &m_ppDevices, &m_cDevices);
		}

		SafeRelease(&pAttributes);

		return hr;
	}


	HRESULT DeviceList::GetDevice(UINT32 index, IMFActivate **ppActivate)
	{
		if (index >= Count())
		{
			return E_INVALIDARG;
		}

		*ppActivate = m_ppDevices[index];
		(*ppActivate)->AddRef();

		return S_OK;
	}

	HRESULT DeviceList::GetDeviceName(UINT32 index, WCHAR **ppszName)
	{
		if (index >= Count())
		{
			return E_INVALIDARG;
		}

		HRESULT hr = S_OK;

		hr = m_ppDevices[index]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			ppszName,
			NULL
			);

		return hr;
	}


	//-------------------------------------------------------------------
	//  CreateInstance
	//
	//  Static class method to create the CCapture object.
	//-------------------------------------------------------------------

	HRESULT CCapture::CreateInstance(
		CCapture **ppCapture // Receives a pointer to the CCapture object.
		)
	{
		if (ppCapture == NULL)
		{
			return E_POINTER;
		}

		CCapture *pCapture = new (std::nothrow) CCapture();

		if (pCapture == NULL)
		{
			return E_OUTOFMEMORY;
		}

		// The CCapture constructor sets the ref count to 1.
		*ppCapture = pCapture;

		return S_OK;
	}


	//-------------------------------------------------------------------
	//  constructor
	//-------------------------------------------------------------------

	CCapture::CCapture() :
	m_pReader(NULL),
		m_nRefCount(1),
		m_pwszSymbolicLink(NULL),
		currentbuffer(0),
		allbuffersexist(FALSE),
		buffersWithNewFramesBitfield(0)
	{
		InitializeCriticalSection(&m_critsec);
	}

	//-------------------------------------------------------------------
	//  destructor
	//-------------------------------------------------------------------

	CCapture::~CCapture()
	{
		assert(m_pReader == NULL);
		DeleteCriticalSection(&m_critsec);
	}


	/////////////// IUnknown methods ///////////////

	//-------------------------------------------------------------------
	//  AddRef
	//-------------------------------------------------------------------

	ULONG CCapture::AddRef()
	{
		return InterlockedIncrement(&m_nRefCount);
	}

	//-------------------------------------------------------------------
	//  Release
	//-------------------------------------------------------------------

	ULONG CCapture::Release()
	{
		ULONG uCount = InterlockedDecrement(&m_nRefCount);
		if (uCount == 0)
		{
			delete this;
		}
		return uCount;
	}

	//-------------------------------------------------------------------
	//  QueryInterface
	//-------------------------------------------------------------------

	HRESULT CCapture::QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(CCapture, IMFSourceReaderCallback),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}


	/////////////// IMFSourceReaderCallback methods ///////////////

	//-------------------------------------------------------------------
	// OnReadSample
	//
	// Called when the IMFMediaSource::ReadSample method completes.
	//-------------------------------------------------------------------

	HRESULT CCapture::OnReadSample(
		HRESULT hrStatus,
		DWORD /*dwStreamIndex*/,
		DWORD /*dwStreamFlags*/,
		LONGLONG /*llTimeStamp*/,
		IMFSample *pSample      // Can be NULL
		)
	{
		LARGE_INTEGER captureTimeStamp;
		QueryPerformanceCounter(&captureTimeStamp); // this is executed <0.1ms after llTimeStamp. should we use llTimeStamp instead?

		EnterCriticalSection(&m_critsec);

		HRESULT hr = S_OK;

		if (FAILED(hrStatus))
		{
			hr = hrStatus;
			goto done;
		}

		if (pSample)
		{
			if(allbuffersexist)
				bufferlist[currentbuffer]->Release();
			HRESULT check = pSample->ConvertToContiguousBuffer(&bufferlist[currentbuffer]);

			if(SUCCEEDED(check))
			{
				bufferCaptureTimeStamp[currentbuffer] = captureTimeStamp;
				buffersWithNewFramesBitfield |= 1 << currentbuffer; // flag this buffer to contain a new frame
				currentbuffer = (currentbuffer + 1) % numBuffers;
				if(currentbuffer == 0)
					allbuffersexist = true;
			}

			if (FAILED(hr)) { goto done; }
		}

		// Read another sample.
		hr = m_pReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			NULL,   // actual
			NULL,   // flags
			NULL,   // timestamp
			NULL    // sample
			);

done:
		LeaveCriticalSection(&m_critsec);
		return hr;
	}


	//-------------------------------------------------------------------
	// OpenMediaSource
	//
	// Set up preview for a specified media source.
	//-------------------------------------------------------------------

	HRESULT CCapture::OpenMediaSource(IMFMediaSource *pSource)
	{
		HRESULT hr = S_OK;
		IMFAttributes *pAttributes = NULL;

		hr = MFCreateAttributes(&pAttributes, 2);

		if (SUCCEEDED(hr))
		{
			hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
		}

		if (SUCCEEDED(hr))
		{
			hr = MFCreateSourceReaderFromMediaSource(
				pSource,
				pAttributes,
				&m_pReader
				);
		}

		SafeRelease(&pAttributes);
		return hr;
	}


	//-------------------------------------------------------------------
	// StartCapture
	//
	// Start capturing.
	//-------------------------------------------------------------------
	HRESULT ConfigureSourceReader(IMFSourceReader *pReader);

	HRESULT CCapture::StartCapture(IMFActivate *pActivate/*, const EncodingParameters& param*/)
	{
		HRESULT hr = S_OK;
		if(!pActivate){
			hr = E_FAIL;
		}


		IMFMediaSource *pSource = NULL;

		EnterCriticalSection(&m_critsec);
		if(SUCCEEDED(hr)){
			// Create the media source for the device.
			hr = pActivate->ActivateObject(
				__uuidof(IMFMediaSource),
				(void**)&pSource
				);
		}

		// Get the symbolic link. This is needed to handle device-
		// loss notifications. (See CheckDeviceLost.)
		if (SUCCEEDED(hr))
		{
			hr = pActivate->GetAllocatedString(
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
				&m_pwszSymbolicLink,
				NULL
				);
		}

		//Set camera parameters
		if (SUCCEEDED(hr))
		{
			CamParametrs test = getParametrs(pSource);
			setParametrs(test,pSource);
			hr = setParams(pSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = OpenMediaSource(pSource);
		}

		// Set up the reader.
		if (SUCCEEDED(hr))
		{
			hr = ConfigureSourceReader(m_pReader);
		}

		if (SUCCEEDED(hr))
		{
			// Request the first video frame.
			hr = m_pReader->ReadSample(
				(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0,
				NULL,
				NULL,
				NULL,
				NULL
				);
		}

		SafeRelease(&pSource);
		LeaveCriticalSection(&m_critsec);
		return hr;
	}


	//-------------------------------------------------------------------
	// EndCaptureSession
	//
	// Stop the capture session.
	//
	// NOTE: This method resets the object's state to State_NotReady.
	// To start another capture session, call SetCaptureFile.
	//-------------------------------------------------------------------

	HRESULT CCapture::EndCaptureSession()
	{
		EnterCriticalSection(&m_critsec);
		SafeRelease(&m_pReader);
		LeaveCriticalSection(&m_critsec);
		return S_OK;
	}


	//-------------------------------------------------------------------
	//  CheckDeviceLost
	//  Checks whether the video capture device was removed.
	//
	//  The application calls this method when is receives a
	//  WM_DEVICECHANGE message.
	//-------------------------------------------------------------------

	HRESULT CCapture::CheckDeviceLost(DEV_BROADCAST_HDR *pHdr, BOOL *pbDeviceLost)
	{
		if (pbDeviceLost == NULL)
		{
			return E_POINTER;
		}

		EnterCriticalSection(&m_critsec);

		*pbDeviceLost = FALSE;

		if (pHdr == NULL)
		{
			goto done;
		}
		if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
		{
			goto done;
		}

		// Compare the device name with the symbolic link.
		DEV_BROADCAST_DEVICEINTERFACE *pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;

		if (m_pwszSymbolicLink)
		{
			size_t len = mbstowcs(nullptr, (char*) &pDi->dbcc_name[0], 0);
			wchar_t* test = new wchar_t[len+1];
			mbstowcs(&test[0], (char*) &pDi->dbcc_name[0], len);
			if (_wcsicmp(m_pwszSymbolicLink, test) == 0)
			{
				*pbDeviceLost = TRUE;
			}
		}

done:
		LeaveCriticalSection(&m_critsec);
		return S_OK;
	}


	/////////////// Private/protected class methods ///////////////

	//-------------------------------------------------------------------
	//  ConfigureSourceReader
	//
	//  Sets the media type on the source reader.
	//-------------------------------------------------------------------
	HRESULT ConfigureSourceReader(IMFSourceReader *pReader)
	{
		HRESULT hr = S_OK;
		GUID subtype = { 0 };
		IMFMediaType *pType = NULL;
		int mediaTypeIndex = 0;

		while(SUCCEEDED(hr))
		{
			hr = pReader->GetNativeMediaType(
				(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				mediaTypeIndex,
				&pType
				);

			if (FAILED(hr)) { goto done; }

			UINT32 width, height;
			hr = ::MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);

			if (FAILED(hr)) { goto done; }

			hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);

			if (FAILED(hr)) { goto done; }

			/*char buf[256];
			sprintf(buf, "%d x %d Guid = {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
			width, height,
			subtype.Data1, subtype.Data2, subtype.Data3,
			subtype.Data4[0], subtype.Data4[1], subtype.Data4[2], subtype.Data4[3],
			subtype.Data4[4], subtype.Data4[5], subtype.Data4[6], subtype.Data4[7]);
			OutputDebugStringA(buf);*/

			if (subtype == MFVideoFormat_RGB24/*MJPG*/ && width == 1280 && height == 960)
			{
				hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
				if (FAILED(hr)) { goto done; }
				break;
			}

			mediaTypeIndex++;
		}

done:
		SafeRelease(&pType);
		return hr;
	}


	//-------------------------------------------------------------------
	// EndCaptureInternal
	//
	// Stops capture.
	//-------------------------------------------------------------------

	HRESULT CCapture::EndCaptureInternal()
	{
		SafeRelease(&m_pReader);
		CoTaskMemFree(m_pwszSymbolicLink);
		m_pwszSymbolicLink = NULL;
		return S_OK;
	}

	//get last image sample
	BYTE* CCapture::getLastImagesample(HRESULT *res, LARGE_INTEGER *captureTimeStamp)
	{
		BYTE *returndata = NULL;
		*res = E_FAIL;

		EnterCriticalSection(&m_critsec);
		int curbuf = currentbuffer == 0 ? numBuffers - 1 : currentbuffer - 1;
		if (buffersWithNewFramesBitfield & (1 << curbuf)) // does the last received buffer contain a new frame?
		{
			buffersWithNewFramesBitfield &= ~(1 << curbuf); // consume the buffer
			bufferlist[curbuf]->Unlock(); // ??
			DWORD len = 0;
			*res = bufferlist[curbuf]->Lock(&returndata, NULL, &len);
			if (captureTimeStamp)
				*captureTimeStamp = bufferCaptureTimeStamp[curbuf];
			bufferlist[curbuf]->Unlock(); // TODO: must be unlocked after use, not here!
		}
		LeaveCriticalSection(&m_critsec);
		return returndata;
	}


	HRESULT CCapture::setParams(IMFMediaSource *vd_pSource)
	{
		HRESULT hr = S_FALSE;
		if(vd_pSource)
		{
			IAMVideoProcAmp *pProcAmp = NULL;
			hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));
			//TODO: Make these configurable?
			long streamPropertyValues[]=
			{
				VideoProcAmp_Brightness, 128, VideoProcAmp_Flags_Manual,
				VideoProcAmp_Contrast, 32, VideoProcAmp_Flags_Manual,
				VideoProcAmp_Saturation, 32, VideoProcAmp_Flags_Manual,
				VideoProcAmp_Sharpness, 32, VideoProcAmp_Flags_Manual,
				VideoProcAmp_WhiteBalance, 6000, VideoProcAmp_Flags_Manual,
				VideoProcAmp_BacklightCompensation, 0, VideoProcAmp_Flags_Manual,
				VideoProcAmp_Gain, 16, VideoProcAmp_Flags_Manual
			};

			for(int i=0;i<7;i++){
				if(SUCCEEDED(hr)){
					hr=pProcAmp->Set(
						streamPropertyValues[i*3], 
						streamPropertyValues[i*3+1], 
						streamPropertyValues[i*3+2]);
				}
			}
			pProcAmp->Release();

			/*IAMCameraControl *pProcControl = NULL;
			hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcControl));

			if (SUCCEEDED(hr))
			{
			long value, flag;
			pProcControl->Get(CameraControl_Exposure,&value,&flag);
			std::cout<<"Value: "<<value<<" Flag: "<<flag<<"\n";
			pProcControl->Release();
			}*/
		}
		return hr;
	}

	void CCapture::setParametrs(CamParametrs parametrs,IMFMediaSource *vd_pSource)
	{
		if(vd_pSource)
		{
			Parametr *pParametr = (Parametr *)(&parametrs);
			IAMVideoProcAmp *pProcAmp = NULL;
			HRESULT hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));

			if (SUCCEEDED(hr))
			{
				for(unsigned int i = 0; i < 10; i++)
				{
					hr = pProcAmp->Set(VideoProcAmp_Brightness + i, pParametr[i].CurrentValue, pParametr[i].Flag);
				}
				pProcAmp->Release();
			}

			IAMCameraControl *pProcControl = NULL;
			hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcControl));

			if (SUCCEEDED(hr))
			{
				for(unsigned int i = 0; i < 7; i++)
				{
					hr = pProcControl->Set(CameraControl_Pan+i, pParametr[10 + i].CurrentValue, pParametr[10 + i].Flag);					
				}
				pProcControl->Release();
			}
		}
	}

	CamParametrs CCapture::getParametrs(IMFMediaSource *vd_pSource)
	{
		CamParametrs out;

		if(vd_pSource)
		{
			Parametr *pParametr = (Parametr *)(&out);
			IAMVideoProcAmp *pProcAmp = NULL;
			HRESULT hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));

			if (SUCCEEDED(hr))
			{
				for(unsigned int i = 0; i < 10; i++)
				{
					Parametr temp;
					hr = pProcAmp->GetRange(VideoProcAmp_Brightness+i, &temp.Min, &temp.Max, &temp.Step, &temp.Default, &temp.Flag);

					if (SUCCEEDED(hr))
					{
						long currentValue = temp.Default;
						hr = pProcAmp->Get(VideoProcAmp_Brightness+i, &currentValue, &temp.Flag);
						temp.CurrentValue = currentValue;
						pParametr[i] = temp;
					}
				}

				pProcAmp->Release();
			}

			IAMCameraControl *pProcControl = NULL;
			hr = vd_pSource->QueryInterface(IID_PPV_ARGS(&pProcControl));

			if (SUCCEEDED(hr))
			{
				for(unsigned int i = 0; i < 7; i++)
				{
					Parametr temp;
					hr = pProcControl->GetRange(CameraControl_Pan+i, &temp.Min, &temp.Max, &temp.Step, &temp.Default, &temp.Flag);

					if (SUCCEEDED(hr))
					{
						long currentValue = temp.Default;
						hr = pProcAmp->Get(CameraControl_Pan+i, &currentValue, &temp.Flag);
						temp.CurrentValue = currentValue;
						pParametr[10 + i] = temp;
					}
				}
				pProcControl->Release();
			}
		}
		return out;
	}

}; // ARLib namespace