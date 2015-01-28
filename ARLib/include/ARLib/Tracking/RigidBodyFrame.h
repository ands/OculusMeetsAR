#ifndef ARLIB_RIGID_BODY_FRAME_H
#define ARLIB_RIGID_BODY_FRAME_H


namespace ARLib{

typedef struct Marker{
	int mMarkerID;
	float mMarkerSize;
	float mX, mY, mZ;
}Marker;

typedef struct RigidBody{
	RigidBody(unsigned int nMarker = 0);
	~RigidBody();
	bool mVisible;
	int mID;
	float mError;
	float mX, mY, mZ;
	float mqX, mqY, mqZ, mqW;
	unsigned int mNMarker;
	Marker *mMarkers;
	void addMarker(unsigned int index, int markerID, float markerSize, float x, float y, float z);
}RigidBody;


typedef struct RBFrame{
	RBFrame(unsigned int nRigidBodys, int frameID, double timestamp, float latency, bool valid, bool Ownership = false);
    RBFrame(const RBFrame& lFrame, const RBFrame& rFrame, float weight);
	~RBFrame();

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

};
#endif
