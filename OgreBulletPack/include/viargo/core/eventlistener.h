
#ifndef VIARGO_EVENT_LISTENER_H
#define VIARGO_EVENT_LISTENER_H

#include <vector>
#include "core/bufferedqueue.h"

namespace viargo {

class Event;
class EventListener;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// atomic execution class
// NOTE: the class is VERY unsave. It's trought for internal use ONLY!!!
class UnitEventTask {

public:
	UnitEventTask(Event* event, EventListener* processor);
	~UnitEventTask();

	void process();
	bool operator>= (const UnitEventTask& rop) const;
	bool operator<= (const UnitEventTask& rop) const;
	bool operator== (const UnitEventTask& rop) const;
	bool operator!= (const UnitEventTask& rop) const;
	bool operator>  (const UnitEventTask& rop) const;
	bool operator<  (const UnitEventTask& rop) const;

private:
	Event* _event;
	EventListener* _processor;
};

/**
 * ???
 */
class EventListener {
public:

	// Constructor
	EventListener(std::string name);

	// Destructor
	virtual ~EventListener();

	// A nil event listener
	static EventListener& nil;

	// Tests two EventListeners for equality. The EventListeners are equal if they have 
	// the same name; all other values doesent matter.
	// @param   a reference to the EventListeners to be compared with <code>this</code>
	// @returns true if <code>this</code> is equal to the EventListeners passed as parameter
	// NOTE     since the EventListeners in viargo all have unique names this operator 
	// only compare the names..
	bool operator == (const EventListener& listener) const;
	
	// Tests two EventListeners for un-equality. 
	// @see <code>operator ==(...)</code>
	bool operator != (const EventListener& listener) const;

	// Gets the name of this listener
	std::string name() const	 {return _name;}

	// Enable/disable listener
	virtual void enable()        {_enabled = true;}
	virtual void disable()       {_enabled = false;}
	virtual bool enabled() const {return _enabled;}

	// Gets called for new events
	// @param: event	the event just received
	// @returns:		true if the event should be accepted (and handled)
	//					false if the event is not handled by this listener
	// NOTE: For optimisation! The function is ment to reduce the number of  
	//		 events processed by this listener by pre-selecting some of them
	// NOTE: Default is to accept all events, may be overwritten by subclasses
	virtual bool onEventReceived(Event* event) {return true;}

	// Gets called for handling of an event if onEventReceived(...) returned true
	// @param: event	the event to be handled
	// @returns:		void
	// NOTE: This function is called for each event in order for the event to be handled
	// NOTE: This function must be implemented by the user
	virtual void handleEvent(Event* event) = 0;

	// Gets called from Viargo.update() to handle frame-specific actions
	// @param:  timeSinceLastUpdate - the time since the last call of the function
	//								  in milliseconds
	// @returns: void
	// NOTE: This function must be implemented by the user
	virtual void update(float timeSinceLastUpdate) = 0;

	// Gets called from the EventManager for a new event. 
	void serveEvent(Event* event);
	
	// Processes the queued events.
	// Gets called from Viargo.update()
	//void processAllEvents();

	// Returns all events since last update()
	// Gets called by Viargo.update()
	std::vector<UnitEventTask> allUnprocessedTasks();

protected:
	bool		_enabled;
	std::string _name;

private:	
	BufferedQueue<Event*> _eventQueue;

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

} // namespace viargo
#endif // VIARGO_EVENT_LISTENER_H
