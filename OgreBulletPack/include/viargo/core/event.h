
#ifndef VIARGO_EVENT_H
#define VIARGO_EVENT_H

#include <string>
#include <typeinfo>

namespace viargo {

/**
 * Base class for events. For Viargo it can be seen as a black box for
 * different kind of information, or as notification of something.
 *
 * Notes:
 * - Events can originate in devices via an EventProvider or be provided
 *   by the application
 * - Events have to be created on the heap (with new operator) and must NEVER be deleted
 * - Events are broadcasted to Metaphors via the EventManager
 * - To prevent deletion of an event and use it outside the <code/>Metaphor::processEvent()
 *   </code> method, the user must call the <code>acquire()</code> method and then the
 *   <code>drop()</code> method, when finisched.
 */
class Event {
	friend class UnitEventTask;

public:
	// Constructor
	Event(std::string device);

	// Gets the name of the source device
	// A reference to the source <code>Device</code> can be retrived by
	// <code>Viargo.device(event.source())</code>
	std::string device() const;

	// Returns the time (in milliseconds) since
	// this event was created.
	float timeSinceCreated() const;

	// Handle multiple references
	void acquire();
	void drop();

protected:
	// prevent creation on the stack
	virtual ~Event();

private:
	int _usageCounter;
	std::string _device;
	float _timeStamp;
};

} // namespace viargo
#endif // VIARGO_EVENT_H
