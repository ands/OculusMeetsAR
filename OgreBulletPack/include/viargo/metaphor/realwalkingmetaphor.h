
#ifndef VIARGO_REAL_WALKING_METAPHOR_H
#define VIARGO_REAL_WALKING_METAPHOR_H

#include "core/metaphor.h"
#include "core/vrgmath.h"

namespace viargo {

/**
 * Simple metaphor implementing real walking
 *
 * Notes:
 * - Uses SensorPositionEvents and SensorOrientationEvents
 * - Does not use the event queue
 *
 * Todo:
 * - Some methods are not yet implemented
 * - Has to be tested
 */
class RealWalkingMetaphor : public Metaphor {
public:
	// Constructor
	RealWalkingMetaphor();

	// Destructor
	~RealWalkingMetaphor() {}

	// Implementation of Metaphor interface
	void handleEvent(Event* event);

	// Implementation of Metaphor interface
	void update(float timeSinceLastUpdate);

private:
	// Helpers
	float periodicMinusAbsolute(float p1, float a) const;
	float periodicMinusPeriodic(float p1, float p2) const;
	void yprToLookUp(vec3 ypr, vec3* look, vec3* up) const;

	// Updated via events
	vec3 _pos;
	vec3 _ypr;

	// True if we got at least one corresponding event
	bool _hasPos;
	bool _hasYpr;

	// Static offsets for zero-alignment
	vec3 _offsetPos;
	vec3 _offsetYpr;

	// Saves of the state to compute relative differences
	vec3 _lastPos;
	vec3 _lastYpr;

	// Saves the current matrix transform
	mat4 _matView;
};

} // namespace viargo
#endif // VIARGO_REAL_WALKING_METAPHOR_H
