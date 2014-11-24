
#ifndef VIARGO_SENSOR_EVENT_H
#define VIARGO_SENSOR_EVENT_H

#include <string>
#include "core/event.h"

namespace viargo {

/**
 * Base class for tracking sensor events. For Viargo it can be seen as
 * a black box for information corresponding to a tracking sensor.
 * 
 * Notes:
 * - Tracking sensors are distinguished via a sensor-Id
 * 
 * Todo:
 * - Add timestamp of tracking data to class
 */
class SensorEvent : public Event {
public:
	// Constructor
	SensorEvent(std::string device, std::string sensorHandle);

	// Destructor
	virtual ~SensorEvent() {}

	// Getter
	std::string sensorHandle() const {return _sensorHandle;}

protected:
	std::string _sensorHandle;
};

} // namespace viargo
#endif // VIARGO_SENSOR_EVENT_H
