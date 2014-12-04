#ifndef ARLIB_RIGID_BODY_FRAME_H
#define ARLIB_RIGID_BODY_FRAME_H

#include "OGRE/Ogre.h"

namespace ARLib{

typedef struct Marker{
	int mMarkerID;
	float mMarkerSize;
	Ogre::Vector3 mMarkerPosition;
}Marker;

typedef struct RigidBody{
	RigidBody(unsigned int nMarker);
	~RigidBody();
	bool mVisible;
	int mID;
	float mError;
	Ogre::Vector3 mPosition;
	Ogre::Quaternion mOrientation;
	unsigned int mNMarker;
	Marker *mMarkers;
	void addMarker(unsigned int index, int markerID, float markerSize, Ogre::Vector3& markerPosition);
}RigidBody;


typedef struct RBFrame{
	RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency);
	~RBFrame();

	void addRigidBody(unsigned int index, RigidBody* rb);

	RigidBody **mRbs;
	bool mChange;
	unsigned int mNRigidBodys;
	int mFrameID;
	double mTimestamp;
	float mLatency;
} RBFrame;

};
#endif
