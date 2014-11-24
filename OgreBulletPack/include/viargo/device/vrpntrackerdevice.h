
#ifndef VRPN_TRACKER_DEVICE_H
#define VRPN_TRACKER_DEVICE_H

#include <map>
#include "device/vrpndevice.h"

class vrpn_Tracker_Remote;

namespace viargo {

/**
 * Class for remote connection with a VRPN tracking server. For Viargo
 * it can be seen as a black box that connects to a VRPN tracking server
 * and generates events when it receives position or orientation data of
 * tracking sensors, which are then broadcasted via the Device.
 * 
 * Notes:
 * - Tracking servers can provide position and/or orientation data
 * 
 * Todo:
 */
class VrpnTrackerDevice : public VrpnDevice {
public:
	enum TrackerType {
		TT_ORIENTATION = 0x01,
		TT_POSITION    = 0x02,
		TT_ORIENTATION_AND_POSITION = TT_ORIENTATION | TT_POSITION
	};

	// Constructor
	VrpnTrackerDevice(std::string name, std::string deviceId = "devicename@localhost",
					  TrackerType type = TT_ORIENTATION_AND_POSITION);

	// Destructor
	virtual ~VrpnTrackerDevice();

	// Implementation of VrpnDevice interface
	virtual void update();

	// Internal - information is sent by VRPN callbacks
	virtual void _handleData(int sensorId, long timeSec, long timeUSec,
							 double posX, double posY, double posZ,
							 double quat1, double quat2, double quat3, double quat4);
	
	void addSensorMapining(int sensorId, std::string sensorHandle);
	void removeSensorMapining(int sensorId);

private:
	vrpn_Tracker_Remote* _tracker;
	TrackerType _type;

	std::string mapSensorId(int sensorId) const;
	typedef std::map<int, std::string> SensorIdMap;
	SensorIdMap _sensorIdMap;
};

} // namespace viargo

#endif // VRPN_TRACKER_DEVICE_H
