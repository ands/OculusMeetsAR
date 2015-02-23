#ifndef ARLIB_RIGID_BODY_FRAME_H
#define ARLIB_RIGID_BODY_FRAME_H

#include <math.h>

namespace ARLib{

typedef struct _Marker{
	int mMarkerID;
	float mMarkerSize;
	float mX, mY, mZ;
}Marker;

typedef struct _RigidBody{
	_RigidBody(unsigned int nMarker = 0, float x = 0.0f, float y=0.0f, float z=0.0f, float qx=0.0f, float qy=0.0f, float qz=0.0f, float qw=1.0f, int id = -1);
	~_RigidBody();
	bool mVisible;
	int mID;
	float mError;
	float mX, mY, mZ;
	float mqX, mqY, mqZ, mqW;
	unsigned int mNMarker;
	Marker *mMarkers;
	void addMarker(unsigned int index, int markerID, float markerSize, float x, float y, float z);
}RigidBody;


typedef struct _RBFrame{
	_RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool Ownership = false);
	~_RBFrame();

	void addRigidBody(unsigned int index, RigidBody* rb);

	RigidBody **mRbs;
	bool mChange;
	unsigned int mNRigidBodys;
	int mFrameID;
	double mTimestamp;
	float mLatency;
    bool mValid;
    bool mOwnership;
} RBFrame;


RigidBody *interpolateRigidBodies(RigidBody *lRigidBody, RigidBody *rRigidBody, float weight);

};
#endif
