#ifndef RIFT_H
#define RIFT_H

#include <vector>

namespace ARLib {

struct DistortionVertex
{
	float ScreenPosNDC[2]; // [-1,+1],[-1,+1] over the entire framebuffer.
    float TimeWarpFactor;  // Lerp factor between time-warp matrices. Can be encoded in Pos.z.
    float VignetteFactor;  // Vignette fade factor. Can be encoded in Pos.w.
    float TanEyeAnglesR[2];
    float TanEyeAnglesG[2];
    float TanEyeAnglesB[2];
};

class Rift
{
	public:
		Rift(int id);
		~Rift();
		
		static void init();
		static void shutdown();
		static bool available(int id);

		void getTextureSizes(
			int *size2dL,
			int *size2dR);

		void getUVScalesAndOffsets(
			float *scale2dL, float *offset2dL,
			float *scale2dR, float *offset2dR);

		void getDistortionGeometries(
			DistortionVertex **verticesL, int *vertexNumL, unsigned short **indicesL, int *indexNumL,
			DistortionVertex **verticesR, int *vertexNumR, unsigned short **indicesR, int *indexNumR);

		void getProjections(
			float znear, float zfar, bool rightHanded,
			float *aspectRatioL, float *projection4x4L,
			float *aspectRatioR, float *projection4x4R);

		void getPose(
			float *position3d,
			float *orientationQuaternionXYZW);

		void recenterPose();

		float getInterpupillaryDistance() { return ipd; }

		bool isPositionCurrentlyTracked() { return positionCurrentlyTracked; }
		bool isOrientationCurrentlyTracked() { return orientationCurrentlyTracked; }
		bool isCameraPoseCurrentlyTracked() { return cameraPoseCurrentlyTracked; }
		bool isPositionTrackingConnected() { return positionTrackingConnected; }

	private:
		static bool isInitialized;

		void initDistortionGeometries();

		const void *hmdHandle;

		float ipd;

		bool positionCurrentlyTracked;
		bool orientationCurrentlyTracked;
		bool cameraPoseCurrentlyTracked;
		bool positionTrackingConnected;

		std::vector<DistortionVertex> vertices[2];
		std::vector<unsigned short> indices[2];
};

}; // ARLib namespace

#endif
