

#ifndef VIARGO_CAMERA_SYSTEM_H
#define VIARGO_CAMERA_SYSTEM_H

#include <string>
#include <map>

#include "core/vrgmath.h"

/**
 * Class for management of all the cameras in the AAL.
 * 
 * Notes:
 * - 
 * 
 * Todo:
 */
namespace viargo{

class Camera;

class CameraSystem {

public:

	// Constructor
	CameraSystem();
	
	// Destructor
	~CameraSystem();

	// Checks if there is a camera in the system
	bool empty() const {return _cameras.empty();}

	// Creates new camera
	void createCamera(std::string name);

	// Destroys a camera
	void destroyCamera(std::string name);

	// Access a camera from the system.
	Camera& camera(std::string name) const;

private:
	typedef std::map<std::string, Camera*> CameraMap;
	CameraMap _cameras;
};

} // namespace viargo
#endif // VIARGO_CAMERA_SYSTEM_H
