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
#define hmd ((ovrHmd)hmdHandle) // TODO: BAD UGLY HACK. EVEN RANDOMLY BREAKS INTELLISENSE. FIND A SANE SOLUTION!!!

Rift::Rift(int id)
{
	if(!isInitialized) throw("Need to initialize first. Call Rift::init()!");
	std::cout << "Creating Rift (ID: " << id << ")" << std::endl;

	hmdHandle = ovrHmd_Create(id);
    if(!hmd) throw("Could not connect to Rift.");

	std::cout << "Oculus Rift found." << std::endl;
	std::cout << "\tProduct Name: " << hmd->ProductName << std::endl;
	std::cout << "\tProduct ID: " << hmd->ProductId << std::endl;
	std::cout << "\tFirmware: " << hmd->FirmwareMajor << "." << hmd->FirmwareMinor << std::endl;
	std::cout << "\tResolution: " << hmd->Resolution.w << "x" << hmd->Resolution.h << std::endl;

	if (!ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0) )
	{
		ovrHmd_Destroy(hmd);
		throw ("\t\tThis Rift does not support the features needed by the application.");
	}

	ipd = ovrHmd_GetFloat(hmd, OVR_KEY_IPD, 0.064f);

	initDistortionGeometries();
}

Rift::~Rift()
{
	ovrHmd_Destroy(hmd);
}

void Rift::getTextureSizes(int *size2dL,
						   int *size2dR)
{
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

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, hmd->DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, hmd->DefaultEyeFov[1]);

	for (int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(hmd, eyeRenderDesc[eyeNum].Eye, eyeRenderDesc[eyeNum].Fov, 0, &meshData);
 
		vertices[eyeNum].resize(meshData.VertexCount);
		indices[eyeNum].resize(meshData.IndexCount);

		memcpy(&vertices[eyeNum][0], &meshData.pVertexData[0], meshData.VertexCount * sizeof(ovrDistortionVertex));
		memcpy(&indices[eyeNum][0], &meshData.pIndexData[0], meshData.IndexCount * sizeof(unsigned short));

		ovrHmd_DestroyDistortionMesh(&meshData);
	}
}

void Rift::getDistortionGeometries(DistortionVertex **verticesL, int *vertexNumL, unsigned short **indicesL, int *indexNumL,
								   DistortionVertex **verticesR, int *vertexNumR, unsigned short **indicesR, int *indexNumR)
{
	*verticesL = &this->vertices[0][0];
	*vertexNumL = this->vertices[0].size();
	*indicesL = &this->indices[0][0];
	*indexNumL = this->indices[0].size();

	*verticesR = &this->vertices[1][0];
	*vertexNumR = this->vertices[1].size();
	*indicesR = &this->indices[1][0];
	*indexNumR = this->indices[1].size();
}

void Rift::getProjections(float znear, float zfar, bool rightHanded,
						  float *aspectRatioL, float *projection4x4L,
						  float *aspectRatioR, float *projection4x4R)
{
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

	/*ovr_WaitTillTime(frameTiming.TimewarpPointSeconds);
	for(int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
		ovrMatrix4f tWM[2];
		ovrHmd_GetEyeTimewarpMatrices(hmd, (ovrEyeType) eyeNum, headPose, tWM);
		Ogre::GpuProgramParametersSharedPtr params = mMatRight->getTechnique(0)->getPass(0)->getVertexProgramParameters();
			params->setNamedConstant("eyeRotationStart",
					Ogre::Matrix4(tWM[0].M[0][0], tWM[0].M[0][1], tWM[0].M[0][2], tWM[0].M[0][3],
						tWM[0].M[1][0], tWM[0].M[1][1], tWM[0].M[1][2], tWM[0].M[1][3],
						tWM[0].M[2][0], tWM[0].M[2][1], tWM[0].M[2][2], tWM[0].M[2][3],
						tWM[0].M[3][0], tWM[0].M[3][1], tWM[0].M[3][2], tWM[0].M[3][3]));
			params->setNamedConstant("eyeRotationEnd",
					Ogre::Matrix4(tWM[1].M[0][0], tWM[1].M[0][1], tWM[1].M[0][2], tWM[1].M[0][3],
						tWM[1].M[1][0], tWM[1].M[1][1], tWM[1].M[1][2], tWM[1].M[1][3],
						tWM[1].M[2][0], tWM[1].M[2][1], tWM[1].M[2][2], tWM[1].M[2][3],
						tWM[1].M[3][0], tWM[1].M[3][1], tWM[1].M[3][2], tWM[1].M[3][3]));
	}*/

	ovrHmd_EndFrameTiming(hmd);
}

void Rift::recenterPose()
{
	ovrHmd_RecenterPose(hmd);
}

#undef hmd

}; // ARLib namespace