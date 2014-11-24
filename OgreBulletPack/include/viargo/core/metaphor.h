
#ifndef VIARGO_METAPHOR_H
#define VIARGO_METAPHOR_H

#include <string>

#include "core/eventlistener.h"

namespace viargo {

/**
 * ???
 * TODO:
 * - clean up the interface by hiding everything else from the EventListener
 */
class Metaphor : public EventListener {
public:
	// Constructor
	Metaphor(std::string name, bool start = true);

	// Destructor
	virtual ~Metaphor() {}

	// Gets called for new events
	// @param: event	the event just received
	// @returns:		true if the event should be accepted (and handled)
	//					false if the event is not handled by this metaphor
	// NOTE: For optimisation! The function is ment to reduce the number of  
	//		 events processed by this metaphor by pre-selecting some of them
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

protected:

};


} // namespace viargo
#endif // VIARGO_METAPHOR_H
