#include <iostream>
#include "OGRE/Ogre.h"
#include "Tracking/TrackingManager.h"
using namespace std;

class SomeRigidBody : public ARLib::RigidBodyEventListener{
public:
	SomeRigidBody(unsigned int iD)
		: RigidBodyEventListener(iD){}

	void onChange(ARLib::RigidBody *rb){
		mVisible = rb->mVisible;
		mPosition = rb->mPosition;
		mOrientation = rb->mOrientation;
	}

	bool mVisible;
	Ogre::Vector3 mPosition;
	Ogre::Quaternion mOrientation;
};


int main(int argc, char* argv[]){
	ARLib::TrackingManager *tManager = new ARLib::TrackingManager(ARLib::ARLIB_NATNET);
	tManager->setNatNetConnectionType(ConnectionType_Multicast);
	tManager->setNatNetServerIP(); //server running on local machine
	tManager->setNatNetClientIP(); //client running on local machine

	ARLib::TRACKING_ERROR_CODE error = tManager->initialize(); //init and check success
	if(error != ARLib::ARLIB_TRACKING_OK){
		cout<<"Failed to Initialize Tracking Manager. ErrorCode:"<<error<<endl;
		return -1;
	}

	//create RigidBody and let it listen to the server
	SomeRigidBody *rBody = new SomeRigidBody(0);
	tManager->registerRigidBodyEventListener(rBody);

	while(true){ 
		tManager->update();
		cout<<rBody->mOrientation<<endl;//show changes
	}

	delete rBody;
	delete tManager;
	return 0;
}