
#ifndef VIARGO_EVENT_MANAGER_H
#define VIARGO_EVENT_MANAGER_H

#include <map>
#include <vector>

namespace viargo {

class Event;
class Device;
class EventListener;

/**
 * Class for dispatching events from EventProviders to EventListeners.
 * For Viargo it can be seen as a black box that manages the event system.
 * Whenever peripheral devices provide new information, this information
 * is broadcasted as event to all EventListeners.
 * 
 * Notes:
 * - Events do not go directly from EventProviders to EventListeners,
 *   all events go through this object
 * 
 * Todo:
 * - 
 */
class EventManager {
public:
	// Constructor
	EventManager();

	// Destructor
	~EventManager();

	// Add/remove device
	void addDevice(Device* device, bool enabled = true);
	void removeDevice(std::string deviceName);

	// Get a device
	Device& device(std::string name);

	// Add/remove event listener
	void addListener(EventListener* listener, int position);
	void removeListener(std::string listenerName);

	// Get an EventListener
	EventListener& listener(std::string listenerName);

	// Dispatch event to event listeners
	void dispatchEvent(Event* event);

	// updates all registered listeners
	void updateAllListener(float timeSinceLastUpdate);

private:
	typedef std::map<std::string, Device*> Devices;
	typedef struct _LST {
		typedef std::map<std::string, EventListener*>::iterator iterator;
		std::map<std::string, EventListener*> _map;
		std::vector<EventListener*> _list;

		void add(EventListener* listener, int position);
		void remove(std::string name);
		EventListener* get(std::string name);
	} Listeners;

	Devices   _devices;
	Listeners _listeners;
};

} // namespace viargo
#endif // VIARGO_EVENT_MANAGER_H
