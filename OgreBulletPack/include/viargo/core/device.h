
#ifndef VIARGO_DEVICE_H
#define VIARGO_DEVICE_H

#include <string>

namespace viargo {

class Event;

/**
 * Abstract base class for connection with devices. For Viargo it can
 * be seen as a black box that handles the connection to one or more
 * input devices. Whenever those devices provide new information,
 * an event is generated and broadcasted via the EventManager.
 *
 * Notes:
 * - Instances of this class are differentiated via "name".
 * - Instances can be enabled (means that events are broadcasted) or disabled
 */
class Device {
public:
	// Constructor
	Device(std::string name);

	// Destructor
	virtual ~Device();

	// Setter
    void enable()  {setEnabled(true);}
    void disable() {setEnabled(false);}

	// Getter
    bool enabled() const     {return _enabled;}
	std::string name() const {return _name;}

	// nil device
	static Device& nil;

	// Tests two devices for equality. The devices are equal if they have
	// the same name; all other values doesent matter.
	// @param   a reference to the device to be compared with <code>this</code>
	// @returns true if <code>this</code> is equal to the device passed as parameter
	// NOTE     since the devices in viargo all have unique names this operator
	// only compare the names..
	bool operator == (const Device& device) const;

	// Tests two devices for un-equality.
	// @see <code>operator ==(...)</code>
	bool operator != (const Device& Device) const;

protected:
	// Broadcasts a new event to the EventManager
	void broadcastEvent(Event* event);

	// Interface - derived classes could override this
	virtual void setEnabled(bool enabled) {
		_enabled = enabled;
	};

	std::string _name;
    bool _enabled;

private:

};

} // namespace viargo
#endif // VIARGO_DEVICE_H
