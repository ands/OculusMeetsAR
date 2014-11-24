#ifndef	VIARGO_TIMER_EVENT_H
#define VIARGO_TIMER_EVENT_H

#include "core/event.h"

namespace viargo {

class TimerEvent : public Event {

public:
	TimerEvent(std::string device, float tickTime)
		:Event(device),
		_tickTime(tickTime)
	{
	}

	~TimerEvent() {
	}
	
	float tickTime() const {return _tickTime;}

private:
	float _tickTime;
};

} // namespace viargo
#endif // VIARGO_TIMER_EVENT_H


