#include "ARLib/Tracking/RigidBodyEventListener.h"

namespace ARLib{

RigidBodyEventListener::RigidBodyEventListener(unsigned int rigidBodyID)
		: mRigidBodyID(rigidBodyID){

}

unsigned int RigidBodyEventListener::getRigidBodyID()const{
	return mRigidBodyID;
}

};