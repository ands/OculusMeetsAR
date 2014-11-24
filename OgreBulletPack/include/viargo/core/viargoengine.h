
#ifndef VIARGO_ENGINE_H
#define VIARGO_ENGINE_H

// core
#include <string>
#include "core/singleton.h"

namespace viargo{

class Timer;
class Device;

class Camera;
class CameraSystem;
class Scene;

class Event;
class Metaphor;
class EventManager;

//-----------------------------------------------------------------------------
// CoreEngine specification
//-----------------------------------------------------------------------------
class ViargoEngineSingleton : public Singleton<ViargoEngineSingleton>{
	friend class Device;
public:

	// referece to this singleton class
	static ViargoEngineSingleton& ref();

	// constructor
	ViargoEngineSingleton();

	// destructor
	~ViargoEngineSingleton();

	// @group AAL
	// Defines the name of the default camera
	static const std::string DEFAULT_CAMERA;

	// Gets the camera system
	CameraSystem& cameraSystem() const;

	// Gets a camera specified by name
	Camera& camera(std::string name = DEFAULT_CAMERA) const;

	// Gets the scene
	Scene&  scene()  const;

	// @group HAL
	// Adds a new hardware device
	void addDevice(Device* device, bool enabled = true);

	// Removes hardware device specified by name
	void removeDevice(std::string name);

	// Gets a device by name
	Device& device(std::string name);

	// @group Engine
	// Constant to indicate that a metaphor must be appendet at the end
	static const int LAST_POSITION = -1;

	// Adds new interaction metaphor
	void addMetaphor(Metaphor* metaphor, int position = LAST_POSITION);

	// Removes interaction metaphor specified by name
	void removeMetaphor(std::string metaphorName);

	// Gets a metaphor by name
	Metaphor& metaphor(std::string metaphorName);

	// Pushes an event in the engine
	void dispatchEvent(Event* event);

	// update the Viargo core (calculates the frame time intern)
	void update();

	// update the Viargo core using user-defined frame time
	void update(float timeSinceLastUpdate);

private:

	CameraSystem* _cameraSystem;
	EventManager* _eventManager;
	Scene*	_scene;
	Timer*  _timer;
};

//-----------------------------------------------------------------------------
// CoreEngine Singleton definition
//-----------------------------------------------------------------------------
// FIXME: THIS IS UGLY!!!
#define Viargo viargo::ViargoEngineSingleton::ref()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace viargo
#endif // VIARGO_ENGINE_H
