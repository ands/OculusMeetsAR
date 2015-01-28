#ifndef ARLIB_RIGID_BODY_EVENT_LISTENER
#define ARLIB_RIGID_BODY_EVENT_LISTENER

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "ARLib/Tracking/RigidBodyFrame.h"

namespace ARLib{

	class RigidBodyEventListener{
	public:
		RigidBodyEventListener(unsigned int rigidBodyID);
		virtual void onChange(RigidBody *rb) = 0;
		unsigned int getRigidBodyID()const;
        bool isRiftListener()const;
	protected:
		unsigned int mRigidBodyID;
        bool mRiftListener;
	};
};
#endif
