#ifndef VIARGO_TIMER_DEVICE_H
#define VIARGO_TIMER_DEVICE_H

#include "core/system.h"
#include "core/device.h"
#include "core/timer.h"

namespace viargo {

/*
* TimerDevice - when enabled generates TimeEvents with a 
* interval defined by 'tickTime' (in milliseconds)
*
* TODO: make it efficient on UNIX!!!
* NOTE: the Device uses busy-waiting thread on non-Win platforms
*/
class TimerDevice : public Device{

public:

	// Constructor
	TimerDevice(std::string name, float tickTime = 1000.0f);
	
	// Destructor
	~TimerDevice();

	// set the time interval to send ticks (milliseconds)
	void  setTickTime(float ms);

	// returns the tick interval
	float tickTime() const;

	// internal use only
	void _tick();
	void _sendTimerEvent();

private:
	Timer _timer;
	float _tickTime;
	System::ThreadHandle _thread;
};

} // namespace viargo
#endif // VIARGO_TIMER_DEVICE_H
