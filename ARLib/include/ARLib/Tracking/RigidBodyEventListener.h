#ifndef ARLIB_RIGID_BODY_EVENT_LISTENER
#define ARLIB_RIGID_BODY_EVENT_LISTENER

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "ARLib/Tracking/RigidBodyFrame.h"
#include "ARLib/Oculus/Rift.h"

namespace ARLib{

	class RigidBodyEventListener{
	public:
		RigidBodyEventListener(unsigned int rigidBodyID, bool isRiftListener);
		virtual void onChange(const RigidBody *rb) = 0;
		unsigned int getRigidBodyID()const;
        bool isRiftListener()const;
		void setReferencePosition(float x, float y, float z);
		void setReferenceOrientation(float qx, float qy, float qz, float qw);
	protected:
		float mRefX, mRefY, mRefZ;
		float mRefQX, mRefQY, mRefQZ, mRefQW;
		unsigned int mRigidBodyID;
        bool mRiftListener;
	};

	class RiftRigidBodyEventListener : public RigidBodyEventListener{
	public:
		RiftRigidBodyEventListener(Rift *mRift, unsigned int rigidBodyID);

		Rift *getRift();
	private:
		Rift *mRift;
	};
};
#endif
