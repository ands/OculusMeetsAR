
#ifndef VIARGO_SENSOR_POSITION_EVENT_H
#define VIARGO_SENSOR_POSITION_EVENT_H

#include "event/sensorevent.h"
#include "core/vrgmath.h"

namespace viargo {

/**
 * Base class for tracking sensor position events. For Viargo it can
 * be seen as a black box for position information corresponding to
 * a tracking sensor.
 * 
 * Notes:
 * - A tracking sensor can provide position and/or orientation events
 */
class SensorPositionEvent : public SensorEvent {
public:
	// Constructor
	SensorPositionEvent(std::string device, std::string sensorHandle, float x, float y, float z);

	// Destructor
	virtual ~SensorPositionEvent() {}

	// Getter
	float x() const  {return _x;}
	float y() const  {return _y;}
	float z() const  {return _z;}
	vec3 pos() const {return vec3(_x, _y, _z);}

private:
	float _x;
	float _y;
	float _z;
};

} // namespace viargo
#endif // VIARGO_SENSOR_POSITION_EVENT_H
