#ifndef ARLIB_RIGID_BODY_EVENT_LISTENER
#define ARLIB_RIGID_BODY_EVENT_LISTENER

/***************************************************
This class manages the client application, which communicates with the Motive stream server,
and the Oculus Rift Handle
***************************************************/

#include "Tracking\RigidBodyFrame.h"

namespace ARLib{

	class RigidBodyEventListener{
	public:
		virtual void onChange() = 0;
	protected:
		bool mVisible;
		Ogre::Vector3 mPosition;
		Ogre::Quaternion mOrientation;
	};
};
#endif
