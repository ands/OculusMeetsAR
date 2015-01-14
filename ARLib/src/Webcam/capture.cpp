#include "ARLib/Webcam/capture.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key);


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
    m_bFirstSample(FALSE),
    m_llBaseTime(0),
    m_pwszSymbolicLink(NULL),
	currentbuffer(0),
	somebufferexist(FALSE),
	allbuffersexist(FALSE)
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
    LONGLONG llTimeStamp,
    IMFSample *pSample      // Can be NULL
    )
{
    EnterCriticalSection(&m_critsec);

    HRESULT hr = S_OK;

    if (FAILED(hrStatus))
    {
        hr = hrStatus;
        goto done;
    }

    if (pSample)
    {
        if (m_bFirstSample)
        {
            m_llBaseTime = llTimeStamp;
            m_bFirstSample = FALSE;
        }

        // rebase the time stamp
        llTimeStamp -= m_llBaseTime;

        hr = pSample->SetSampleTime(llTimeStamp);

		if(currentbuffer==0 && somebufferexist){
			allbuffersexist=TRUE;
		}
		if(allbuffersexist){
			bufferlist[currentbuffer]->Release();
		}
		HRESULT check = pSample->ConvertToContiguousBuffer(&bufferlist[currentbuffer]);

		if(SUCCEEDED(check)){
			currentbuffer=(currentbuffer+1)%10;

			if(currentbuffer==3){
				somebufferexist=TRUE;
			}

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

HRESULT CCapture::StartCapture(
    IMFActivate *pActivate,
    const EncodingParameters& param
    )
{
    HRESULT hr = S_OK;

    IMFMediaSource *pSource = NULL;

    EnterCriticalSection(&m_critsec);

    // Create the media source for the device.
    hr = pActivate->ActivateObject(
        __uuidof(IMFMediaSource),
        (void**)&pSource
        );

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

    if (SUCCEEDED(hr))
    {
        hr = OpenMediaSource(pSource);
    }

    // Set up the encoding parameters.
    if (SUCCEEDED(hr))
    {
        hr = ConfigureCapture(param);
    }

    if (SUCCEEDED(hr))
    {
        m_bFirstSample = TRUE;
        m_llBaseTime = 0;

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

    HRESULT hr = S_OK;

    SafeRelease(&m_pReader);

    LeaveCriticalSection(&m_critsec);

    return hr;
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

    DEV_BROADCAST_DEVICEINTERFACE *pDi = NULL;

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

    pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;

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
// ConfigureCapture
//
// Configures the capture session.
//
//-------------------------------------------------------------------

HRESULT CCapture::ConfigureCapture(const EncodingParameters& param)
{
    HRESULT hr = S_OK;
    DWORD sink_stream = 0;

    IMFMediaType *pType = NULL;

    hr = ConfigureSourceReader(m_pReader);

    if (SUCCEEDED(hr))
    {
        hr = m_pReader->GetCurrentMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            &pType
            );
    }

    if (SUCCEEDED(hr))
    {
        // Register the color converter DSP for this process, in the video
        // processor category. This will enable the sink writer to enumerate
        // the color converter when the sink writer attempts to match the
        // media types.

        hr = MFTRegisterLocalByCLSID(
            __uuidof(CColorConvertDMO),
            MFT_CATEGORY_VIDEO_PROCESSOR,
            L"",
            MFT_ENUM_FLAG_SYNCMFT,
            0,
            NULL,
            0,
            NULL
            );
    }

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
    HRESULT hr = S_OK;

    SafeRelease(&m_pReader);

    CoTaskMemFree(m_pwszSymbolicLink);
    m_pwszSymbolicLink = NULL;

    return hr;
}

//get last image sample
BYTE* CCapture::getLastImagesample(HRESULT *res){
	BYTE *returndata=NULL;
	*res = E_FAIL;
	if(somebufferexist){
		int curbuf=0;
		if(currentbuffer==0){
			curbuf=9;
		}
		else{
			curbuf = (currentbuffer-1)%10;
		}
		bufferlist[curbuf]->Unlock(); // ??

		//DWORD length = 0;
		*res = bufferlist[curbuf]->Lock(&returndata,NULL,NULL/*&length*/);
		/*char buf[256];
		sprintf(buf, "%d/960 = %d; %d/960/3 = %d\n", length, length / 960, length, length / 960 / 3);
		OutputDebugStringA(buf);*/

		bufferlist[curbuf]->Unlock(); // TODO: muss nach der nutzung ausgeführt werden, nicht hier!
	}
	return returndata;
}



//-------------------------------------------------------------------
// CopyAttribute
//
// Copy an attribute value from one attribute store to another.
//-------------------------------------------------------------------

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key)
{
    PROPVARIANT var;
    PropVariantInit(&var);

    HRESULT hr = S_OK;

    hr = pSrc->GetItem(key, &var);
    if (SUCCEEDED(hr))
    {
        hr = pDest->SetItem(key, var);
    }

    PropVariantClear(&var);
    return hr;
}
