
#ifndef VIARGO_VRPN_DEVICE_H
#define VIARGO_VRPN_DEVICE_H

#include <vector>
#include "core/device.h"

namespace viargo {

// forward declaration
class VrpnConnectionManager;

/**
 * Class for connection with devices over network via VRPN.
 * For Viargo it can be seen as a black box that handles the connection
 * to one or more devices that implemented the VRPN server protocol.
 * Whenever those devices provide information, the object generates
 * an event, which is then broadcasted via the EventManager.
 * 
 * Notes:
 * - VRPN devices and servers have to implement the exact same protocol
 * - Parameters deviceId should be in the format <server_name>@<host_ip>
 * 
 * Todo:
 * - The class is not (yet) meant for bi-directional communication with devices
 */
class VrpnDevice : public Device {
public:
	// Constructors
	VrpnDevice(std::string name, std::string deviceId = "deviceName@localhost");

	// Destructor
	virtual ~VrpnDevice();

	// Updates are triggered from the VRPN thread loop
	virtual void update() {}

	// Returns an identifier for this device
	std::string deviceId() const { return _deviceId; }

private:
	static VrpnConnectionManager* _vrpnConnectionManager;
	std::string _deviceId;
};

} // namespace viargo

#endif // VIARGO_VRPN_DEVICE_H
