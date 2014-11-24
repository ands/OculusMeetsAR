
#ifndef VIARGO_EOS_DEVICE_H
#define VIARGO_EOS_DEVICE_H

#include <string>
#include <map>

#include "device/networkdevice.h"

#include "event/sensororientationevent.h"
#include "event/sensorpositionevent.h"

namespace viargo {

/**
 * Class for parsing of EOS-Tracker packets. For Viargo it can be seen as a black
 * box that broadcasts events via the EventProvider, which originate in EOS Tracker.
 * 
 * Notes:
 * - One parsed packet may result in multiple events
 */
class EosDevice : public NetworkDevice {
public:
    /// Constructor
	EosDevice(std::string name, std::string host, int port);

    /// Destructor
	virtual ~EosDevice();

	/// Implementation of NetworkDevice interface
	void parseData(const char* data, int size);

private:
	float bigToLittleEndian(float f);
	std::string mapSensorHandle(std::string handle);
};

} // namespace viargo
#endif // VIARGO_EOS_DEVICE_H
