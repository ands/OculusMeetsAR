#ifndef ARLIB_RIGID_BODY_FRAME_H
#define ARLIB_RIGID_BODY_FRAME_H

#include <math.h>

namespace ARLib{

struct Marker{
	int mMarkerID;
	float mMarkerSize;
	float mX, mY, mZ;
};

struct RigidBody{
	RigidBody(int id = -1, float x = 0.0f, float y=0.0f, float z=0.0f, float qx=0.0f, float qy=0.0f, float qz=0.0f, float qw=1.0f);
	bool mVisible;
	int mID;
	float mX, mY, mZ;
	float mqX, mqY, mqZ, mqW;
};


struct RBFrame{
	RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool Ownership = false);
	~RBFrame();

	RigidBody*& operator[](unsigned int);

	RigidBody **mRbs;
	unsigned int mNRigidBodies;
	int mFrameID;
	double mTimestamp;
	float mLatency;
    bool mValid;
	bool mOwnership;
};


RigidBody *interpolateRigidBodies(RigidBody *lRigidBody, RigidBody *rRigidBody, float weight);

};
#endif
