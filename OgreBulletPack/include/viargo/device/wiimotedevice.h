
#ifndef VIARGO_WIIMOTE_DEVICE_H
#define VIARGO_WIIMOTE_DEVICE_H

#include "device/networkdevice.h"
#include "event/wiimoteevent.h"

namespace viargo {

class BinaryDataReader;

/**
 * Class for parsing of packets sent by the Viargo sample server for Nintendo
 * Wiimotes, Nunchucks and BalanceBoards. For Viargo it can be seen as a black
 * box that broadcasts events via the EventProvider, which originate in these
 * devices.
 * 
 * Notes:
 * - One packet may result in multiple events
 */
class WiimoteDevice : public NetworkDevice {
public:
	// Constructor
	WiimoteDevice(std::string name, std::string host, int port);

	// Destructor
	virtual ~WiimoteDevice();

	// Implementation of DataParser interface
	void parseData(const char* data, int size);

private:
	void decodeWii(std::string sourceId, BinaryDataReader& bdr);
	void decodeNunch(std::string sourceId, BinaryDataReader& bdr);
	void decodeBalanceBoard(std::string sourceId, BinaryDataReader& bdr);
	void decodeAccelerometer(WiimoteEvent::EventSourceType, std::string sourceId, BinaryDataReader& bdr);
	void decodeJoystick(WiimoteEvent::EventSourceType, std::string sourceId, BinaryDataReader& bdr);
};

} // namespace viargo
#endif // VIARGO_WIIMOTE_DEVICE_H
