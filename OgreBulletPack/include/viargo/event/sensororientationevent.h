
#ifndef VIARGO_SENSOR_ORIENTATION_EVENT_H
#define VIARGO_SENSOR_ORIENTATION_EVENT_H

#include "event/sensorevent.h"
#include "core/vrgmath.h"

namespace viargo {

/**
 * Base class for tracking sensor orientation events. For Viargo it
 * can be seen as a black box for orientation information corresponding
 * to a tracking sensor.
 * 
 * Notes:
 * - A tracking sensor can provide position and/or orientation events
 */
class SensorOrientationEvent : public SensorEvent {
public:
	// Constructor
	SensorOrientationEvent(std::string device, std::string sensorHandle, float yaw, float pitch, float roll);

	// Destructor
	virtual ~SensorOrientationEvent() {}

	// Getter
	float yaw() const   {return _yaw;}
	float pitch() const {return _pitch;}
	float roll() const  {return _roll;}
	vec3 ypr() const    {return vec3(_yaw, _pitch, _roll);}

private:
	float _yaw;
	float _pitch;
	float _roll;
};

} // namespace viargo
#endif // VIARGO_SENSOR_ORIENTATION_EVENT_H
