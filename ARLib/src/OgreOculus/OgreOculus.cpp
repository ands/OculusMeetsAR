#include "OVR_CAPI.h"

/*ovrHmd             HMD = 0;
ovrEyeRenderDesc   EyeRenderDesc[2];
ovrRecti           EyeRenderViewport[2];

#if SDK_RENDER
#define OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"
ovrD3D11Texture    EyeTexture[2];
#else
ShaderSet *         Shaders;
ID3D11InputLayout * VertexIL;
Ptr<Buffer>         MeshVBs[2];
Ptr<Buffer>         MeshIBs[2];
ovrVector2f         UVScaleOffset[2][2];
#endif

void initOculus()
{
	// Initializes LibOVR, and the Rift
	ovr_Initialize();
	if (!HMD)
	{
		HMD = ovrHmd_Create(0);
		if (!HMD)
		{
			MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK);
			return(1);
		}
		if (HMD->ProductName[0] == '\0')
			MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);
	}
}*/