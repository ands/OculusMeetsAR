#ifndef ARLIB_CAPTURE_H
#define ARLIB_CAPTURE_H

#include <new>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Wmcodecdsp.h>
#include <assert.h>
#include <Dbt.h>
#include <shlwapi.h>

//Class for enumerating videocapture devices
class DeviceList
{
public:
	UINT32      m_cDevices;
    IMFActivate **m_ppDevices;

    DeviceList() : m_ppDevices(NULL), m_cDevices(0)
    {

    }
    ~DeviceList()
    {
        Clear();
    }

    UINT32  Count() const { return m_cDevices; }

    void    Clear();
    HRESULT EnumerateDevices();
    HRESULT GetDevice(UINT32 index, IMFActivate **ppActivate);
    HRESULT GetDeviceName(UINT32 index, WCHAR **ppszName);
};

class CCapture : public IMFSourceReaderCallback
{
public:
    static HRESULT CreateInstance(
        CCapture **ppPlayer
    );

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFSourceReaderCallback methods
    STDMETHODIMP OnReadSample(
        HRESULT hrStatus,
        DWORD dwStreamIndex,
        DWORD dwStreamFlags,
        LONGLONG llTimestamp,
        IMFSample *pSample
    );

    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
    {
        return S_OK;
    }

    STDMETHODIMP OnFlush(DWORD)
    {
        return S_OK;
    }

    HRESULT     StartCapture(IMFActivate *pActivate);
    HRESULT     EndCaptureSession();
    HRESULT     CheckDeviceLost(DEV_BROADCAST_HDR *pHdr, BOOL *pbDeviceLost);
	BYTE*       CCapture::getLastImagesample(HRESULT *res);
	bool somebufferexist;

protected:

    // Constructor is private. Use static CreateInstance method to instantiate.
    CCapture();

    // Destructor is private. Caller should call Release.
    virtual ~CCapture();

    HRESULT OpenMediaSource(IMFMediaSource *pSource);
    HRESULT EndCaptureInternal();

    long                    m_nRefCount;        // Reference count.
    CRITICAL_SECTION        m_critsec;
    IMFSourceReader         *m_pReader;
    WCHAR                   *m_pwszSymbolicLink;

	//capturesamples
	#define BUFFER_NUM 10
	IMFMediaBuffer *bufferlist[BUFFER_NUM];
	int currentbuffer;
	bool allbuffersexist;
};

#endif