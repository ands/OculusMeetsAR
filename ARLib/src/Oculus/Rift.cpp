#include <iostream> // TODO: replace this with the log file!
#include "OVR.h"
#include "ARLib/Oculus/Rift.h"

namespace ARLib {

/////////////////////////////////////////////////////////////////////////
// statics

bool Rift::isInitialized = false;

void Rift::init()
{
	if(!isInitialized)
	{
		ovr_Initialize();
		isInitialized = true;
	}
}
void Rift::shutdown()
{
	if(isInitialized)
	{
		ovr_Shutdown();
		isInitialized = false;
	}
}
bool Rift::available(int id)
{
	if(!isInitialized) throw("Need to initialize first. Call Rift::init()!");
	ovrHmd hmd = ovrHmd_Create(id);
	bool result = hmd ? true : false;
	ovrHmd_Destroy(hmd);
	return result;
}

/////////////////////////////////////////////////////////////////////////
// members
Rift::Rift(int id)
	: hmdHandle(nullptr)
	, positionCurrentlyTracked(false)
	, orientationCurrentlyTracked(false)
	, cameraPoseCurrentlyTracked(false)
	, positionTrackingConnected(false)
{
	// initialize arrays
	verticesNum[0] = verticesNum[1] = 0;
	vertices[0] = vertices[1] = nullptr;
	indicesNum[0] = indicesNum[1] = 0;
	indices[0] = indices[1] = nullptr;

	// initialize hmd
	if(!isInitialized) throw("Need to initialize first. Call Rift::init()!");
	std::cout << "Creating Rift (ID: " << id << ")" << std::endl;

	hmdHandle = ovrHmd_Create(id);
    if(!hmdHandle) throw("Could not connect to Rift.");

	ovrHmd hmd = ((ovrHmd)hmdHandle);
	std::cout << "Oculus Rift found." << std::endl;
	std::cout << "\tProduct Name: " << hmd->ProductName << std::endl;
	std::cout << "\tProduct ID: " << hmd->ProductId << std::endl;
	std::cout << "\tFirmware: " << hmd->FirmwareMajor << "." << hmd->FirmwareMinor << std::endl;
	std::cout << "\tResolution: " << hmd->Resolution.w << "x" << hmd->Resolution.h << std::endl;

    ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_NoVSync | ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

	/*ovrRenderAPIConfig config;
    unsigned distortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette;
    //distortionCaps |= ovrDistortionCap_SRGB; // ?
	distortionCaps |= ovrDistortionCap_Overdrive;
    distortionCaps |= ovrDistortionCap_TimeWarp;
    //distortionCaps |= ovrDistortionCap_ProfileNoTimewarpSpinWaits; // TimewarpNoJitEnabled
    //distortionCaps |= ovrDistortionCap_HqDistortion; // only if antialiasing is enabled
	ovrEyeRenderDesc eyeRenderDesc[2];
    if (!ovrHmd_ConfigureRendering(hmd, &config, distortionCaps, hmd->DefaultEyeFov, eyeRenderDesc))
    {
        throw ("This Rift does not support the rendering features needed by the application.");
    }*/

	if (!ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0))
	{
		ovrHmd_Destroy(hmd);
		throw ("This Rift does not support the tracking features needed by the application.");
	}

	ipd = ovrHmd_GetFloat(hmd, OVR_KEY_IPD, 0.064f);

	initDistortionGeometries();
}

Rift::~Rift()
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);
	ovrHmd_Destroy(hmd);

	if (vertices[0])
		delete vertices[0];
	if (vertices[1])
		delete vertices[1];
	if (indices[0])
		delete indices[0];
	if (indices[1])
		delete indices[1];
}

void Rift::getTextureSizes(int *size2dL,
						   int *size2dR)
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);

	OVR::Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
	size2dL[0] = recommendedTex0Size.w;
	size2dL[1] = recommendedTex0Size.h;

	OVR::Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);
	size2dR[0] = recommendedTex1Size.w;
	size2dR[1] = recommendedTex1Size.h;
}

void Rift::getUVScalesAndOffsets(float *scale2dL, float *offset2dL,
								 float *scale2dR, float *offset2dR)
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, hmd->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, hmd->DefaultEyeFov[1]);

	ovrRecti viewports[2];
	viewports[0].Pos.x = 0;
	viewports[0].Pos.y = 0;
	viewports[0].Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
	viewports[1].Pos.x = viewports[0].Size.w;
	viewports[1].Pos.y = 0;
	viewports[1].Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);

	ovrVector2f UVScaleOffset[2][2];
	ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[0].Fov, viewports[0].Size, viewports[0], UVScaleOffset[0]);
	ovrHmd_GetRenderScaleAndOffset(eyeRenderDesc[1].Fov, viewports[1].Size, viewports[1], UVScaleOffset[1]);

	scale2dL[0] = UVScaleOffset[0][0].x;
	scale2dL[1] = UVScaleOffset[0][0].y;
	scale2dR[0] = UVScaleOffset[1][0].x;
	scale2dR[1] = UVScaleOffset[1][0].y;

	offset2dL[0] = UVScaleOffset[0][1].x;
	offset2dL[1] = UVScaleOffset[0][1].y;
	offset2dR[0] = UVScaleOffset[1][1].x;
	offset2dR[1] = UVScaleOffset[1][1].y;
}

void Rift::initDistortionGeometries()
{
	assert(sizeof(ovrDistortionVertex) == sizeof(DistortionVertex));

	ovrHmd hmd = ((ovrHmd)hmdHandle);
	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, hmd->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, hmd->DefaultEyeFov[1]);

	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(hmd, eyeRenderDesc[eyeNum].Eye, eyeRenderDesc[eyeNum].Fov, 0, &meshData);
 
		verticesNum[eyeNum] = meshData.VertexCount;
		indicesNum[eyeNum] = meshData.IndexCount;

		vertices[eyeNum] = new DistortionVertex[verticesNum[eyeNum]];
		indices[eyeNum] = new unsigned short[indicesNum[eyeNum]];

		memcpy(vertices[eyeNum], meshData.pVertexData, verticesNum[eyeNum] * sizeof(ovrDistortionVertex));
		memcpy(indices[eyeNum], meshData.pIndexData, indicesNum[eyeNum] * sizeof(unsigned short));

		ovrHmd_DestroyDistortionMesh(&meshData);
	}
}

void Rift::getDistortionGeometries(DistortionVertex **verticesL, int *vertexNumL, unsigned short **indicesL, int *indexNumL,
								   DistortionVertex **verticesR, int *vertexNumR, unsigned short **indicesR, int *indexNumR)
{
	*verticesL = vertices[0];
	*vertexNumL = verticesNum[0];
	*indicesL = indices[0];
	*indexNumL = indicesNum[0];

	*verticesR = vertices[1];
	*vertexNumR = verticesNum[1];
	*indicesR = indices[1];
	*indexNumR = indicesNum[1];
}

void Rift::getProjections(float znear, float zfar, bool rightHanded,
						  float *aspectRatioL, float *projection4x4L,
						  float *aspectRatioR, float *projection4x4R)
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);

	ovrFovPort fovLeft = hmd->DefaultEyeFov[ovrEye_Left];
	ovrFovPort fovRight = hmd->DefaultEyeFov[ovrEye_Right];

	float combinedTanHalfFovHorizontalL = std::max(fovLeft.LeftTan, fovLeft.RightTan);
	float combinedTanHalfFovHorizontalR = std::max(fovRight.LeftTan, fovRight.RightTan);

	float combinedTanHalfFovVerticalL = std::max(fovLeft.UpTan, fovLeft.DownTan);
	float combinedTanHalfFovVerticalR = std::max(fovRight.UpTan, fovRight.DownTan);

	*aspectRatioL = combinedTanHalfFovHorizontalL / combinedTanHalfFovVerticalL;
	*aspectRatioR = combinedTanHalfFovHorizontalR / combinedTanHalfFovVerticalR;
	
	ovrMatrix4f projectionL = ovrMatrix4f_Projection(fovLeft, znear, zfar, rightHanded);
	ovrMatrix4f projectionR = ovrMatrix4f_Projection(fovRight, znear, zfar, rightHanded);

	memcpy(projection4x4L, projectionL.M, 16 * sizeof(float));
	memcpy(projection4x4R, projectionR.M, 16 * sizeof(float));
}

void Rift::getPose(float *position3d, float *orientationQuaternionXYZW)
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);
	ovrFrameTiming frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);

	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, frameTiming.ScanoutMidpointSeconds);
	positionCurrentlyTracked = (ts.StatusFlags & ovrStatus_PositionTracked) != 0;
	orientationCurrentlyTracked = (ts.StatusFlags & ovrStatus_OrientationTracked) != 0;
	cameraPoseCurrentlyTracked = (ts.StatusFlags & ovrStatus_CameraPoseTracked) != 0;
	positionTrackingConnected = (ts.StatusFlags & ovrStatus_PositionConnected) != 0;

	ovrPosef pose = ts.HeadPose.ThePose;
	position3d[0] = pose.Position.x;
	position3d[1] = pose.Position.y;
	position3d[2] = pose.Position.z;
	orientationQuaternionXYZW[0] = pose.Orientation.x;
	orientationQuaternionXYZW[1] = pose.Orientation.y;
	orientationQuaternionXYZW[2] = pose.Orientation.z;
	orientationQuaternionXYZW[3] = pose.Orientation.w;

	//ovr_WaitTillTime(frameTiming.TimewarpPointSeconds);
	/*ovrMatrix4f m[2];
	ovrHmd_GetEyeTimewarpMatrices(hmd, ovrEye_Left, pose, m);
	memcpy(eyeRotationStart[0], &m[0].M[0][0], 16 * sizeof(float));
	memcpy(eyeRotationEnd  [0], &m[1].M[0][0], 16 * sizeof(float));
	ovrHmd_GetEyeTimewarpMatrices(hmd, ovrEye_Right, pose, m);
	memcpy(eyeRotationStart[1], &m[0].M[0][0], 16 * sizeof(float));
	memcpy(eyeRotationEnd  [1], &m[1].M[0][0], 16 * sizeof(float));*/

	ovrHmd_EndFrameTiming(hmd);
}

/*void Rift::getTimewarpMatrices(float *rotationStart4x4L, float *rotationEnd4x4L,
							   float *rotationStart4x4R, float *rotationEnd4x4R)
{
	memcpy(rotationStart4x4L, eyeRotationStart[0], 16 * sizeof(float));
	memcpy(rotationEnd4x4L  , eyeRotationEnd  [0], 16 * sizeof(float));
	memcpy(rotationStart4x4R, eyeRotationStart[1], 16 * sizeof(float));
	memcpy(rotationEnd4x4R  , eyeRotationEnd  [1], 16 * sizeof(float));
}*/

void Rift::recenterPose()
{
	ovrHmd hmd = ((ovrHmd)hmdHandle);
	ovrHmd_RecenterPose(hmd);
}

#undef hmd

}; // ARLib namespace