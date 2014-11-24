#ifndef VIARGO_TIMER_H
#define VIARGO_TIMER_H

namespace viargo {

class Timer {
public:
	// Constructor
	Timer();

	// Destructor
	~Timer();

	// resets the timer
	void reset();

	// gets the time (in milliseconds) since the last call of frame()
	float frame();

	// gets the time (in milliseconds) since the last reset
	float runTime() const;

	// busy waiting for ms milliseconds
	void wait(float ms) const;

private:
	float _resetTime;
	float _lastFrameTime;
};

} // namespace viargo
#endif // VIARGO_TIMER_H
