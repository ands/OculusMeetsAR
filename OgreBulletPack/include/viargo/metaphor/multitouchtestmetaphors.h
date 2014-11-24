#ifndef MULTI_TOUCH_TEST_METAPHORS_H
#define MULTI_TOUCH_TEST_METAPHORS_H

#include <viargo.h>

namespace viargo {
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Very simple MT-metaphor that only prints the parameters of the
// incomming events to std. 
class PrintEventMetaphor : public Metaphor {

public:

	// Constructor
	PrintEventMetaphor(std::string name) 
		:Metaphor(name) 
	{
	}

	// Destructor
	~PrintEventMetaphor() {
	}

	// Gets called for new events
	// @param: event	the event just received
	// @returns:		true if the event should be accepted (and handled)
	//					false if the event is not handled by this metaphor
	// NOTE: For optimisation! The function is ment to reduce the number of  
	//		 events processed by this metaphor by pre-selecting some of them
	// NOTE: Default is to accept all events, may be overwritten by subclasses
	bool onEventReceived(Event* event) {
		// accept ONLY MT-events
		if (typeid(*event) == typeid(RawMultiTouchEvent))
			return true;
		// ignore the rest
		return false;
	}
	
	// Gets called for handling of an event if onEventReceived(...) returned true
	// @param: event	the event to be handled
	// @returns:		void
	// NOTE: This function is called for each event in order for the event to be handled
	void handleEvent(Event* event) {
	
		// there are only MT events to be processed, but check it anyway
		if (typeid(*event) == typeid(RawMultiTouchEvent)) {
			RawMultiTouchEvent* mte = (RawMultiTouchEvent*)event; // it's save to do this :)

			// who has sended the event 
			std::cout << "\n" << mte->device() << ": ";

			// dims
			// NOTE: currently only 2D objects are supported by the hardware!
			if (mte->is2D())
				std::cout << "2D ";
			else if (mte->is3D())
				std::cout << "3D ";
			else // if (mte->is25D())
				std::cout << "2.5D ";

			// finger(cursor) vs. prop(fiducial/object)
			// NOTE: currently only fingers (cursors) are detected by the hardware!
			if (mte->isCursor())
				std::cout << "finger ";
			else // if(mte->isObject())
				std::cout << "prop(" << mte->fiducalID() << ") "; // props have an unique class-id -> fiducial id


			// action
			// speed, acceleration are ignored, but you can use them if you wish
			// angle, rotation speed and acceleration are only defined for props.. not supported by our hardware
			switch (mte->action()) {
				// dummy.. should never appear
				case RawMultiTouchEvent::INVALID_ACTION:
					std::cout << "INVALID_ACTION ";
					break;
				// everything btw. two frame events (with different frame-ids)
				// has happened simultaneously.. FRAME1 ADD1 ADD2 UPDATE1 REMOVE2 FRAME2 UPDATE1 ADD2... 
				case RawMultiTouchEvent::FRAME:
					std::cout << "FRAME (" << mte->sessionID() << ") ";
					break;
				// new finger/prop was detected
				case RawMultiTouchEvent::APPEARED:
					std::cout << "APPEARED (" << mte->sessionID() << ") at " << mte->position();
					break;
				// a finger/prop has disappeared
				case RawMultiTouchEvent::DISAPPEARED:
					std::cout << "DISAPPEARED (" << mte->sessionID() << ") ";
					break;
				// a finger's/prop's position (and/or) orientation has changed
				case RawMultiTouchEvent::UPDATED:
					std::cout << "UPDATED (" << mte->sessionID() << ") to " << mte->position();
					break;
			}
		}
	}

	// Gets called from Viargo.update() to handle frame-specific actions
	// @param:  timeSinceLastUpdate - the time since the last call of the function
	//								  in milliseconds
	// @returns: void
	void update(float timeSinceLastUpdate) {
		// nothing to do here...
		// you will usually collect the MT state in the handleEvent() function
		// and perform the actions (manipulate cameras or objects) here...
		
		//std::cout << "\n" << timeSinceLastUpdate << "ms passed since the last update";
	}

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class PrintMultiTouchMetaphor : public Metaphor {

public:

	// Constructor
	PrintMultiTouchMetaphor(std::string name) 
		:Metaphor(name) 
	{
	}

	// Destructor
	~PrintMultiTouchMetaphor() {
	}

	// Gets called for new events
	bool onEventReceived(Event* event) {
		// accept ONLY MT-events
		if (typeid(*event) == typeid(MultiTouchEvent))
			return true;

		// ignore the rest
		return false;
	}
	
	// Gets called for handling of an event if onEventReceived(...) returned true
	void handleEvent(Event* event) {

		MultiTouchEvent& mte = *((MultiTouchEvent*)event); // safe

		// who has sended the event 
		std::cout << "\n" << mte.device() << ": {";
		if (!mte.updated().empty()) {
			std::cout << (mte.updated())[0];
			for (size_t i = 1; i < mte.updated().size(); ++i) {
				std::cout << ", " << (mte.updated())[i];
			}
			std::cout << "}";
		}

		std::cout << "\nmap: { ";
		for (mte.touches().resetIterator(); mte.touches().hasNext(); /*NOP*/) {
			viargo::Touch& touch = mte.touches().next();
			std::cout << "\n\t[" << touch.id << ", " << touch.classId << ", " << touch.userId << "]";
			std::cout << "\n\t" << touch.current.position;
			std::cout << "\n\t" << touch.current.angle;
			std::cout << "\n\t" << touch.tag.size();
		}
		std::cout << "\n}";
	}

	// Gets called from Viargo.update() to handle frame-specific actions
	void update(float timeSinceLastUpdate) {

	}

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Simple metaphor that only moves the camera in x/y axes
class CameraMoveMetaphor : public Metaphor {

public:

	// Constructor
	CameraMoveMetaphor(std::string name) 
		:Metaphor(name) 
	{
	}

	// Destructor
	~CameraMoveMetaphor() {
	}

	void handleEvent(Event* event) {
		if (typeid(*event) == typeid(RawMultiTouchEvent)) {
			RawMultiTouchEvent* mte = (RawMultiTouchEvent*)event; // it's save to do this :)

			// we are only interested in 2D finger touches!
			if (!mte->is2D() || !mte->isCursor())
				return;

			// only put/update all the touches in the _currentTouches map. 
			// they will by processed in update()
			switch (mte->action()) {
				 // ignore
				case RawMultiTouchEvent::INVALID_ACTION:
					return;
				
				// ignore
				case RawMultiTouchEvent::FRAME:
					return;

				// add new touch
				case RawMultiTouchEvent::APPEARED:{
					_currentTouches.insert(std::make_pair(mte->sessionID(), mte->position()));
					break;
				}
				// remove a touch
				case RawMultiTouchEvent::DISAPPEARED:{
					TouchMap::iterator it = _currentTouches.find(mte->sessionID());
					if (it != _currentTouches.end())
						_currentTouches.erase(it);
					break;
				}
				// update touch position
				case RawMultiTouchEvent::UPDATED:{
					TouchMap::iterator it = _currentTouches.find(mte->sessionID());
					if (it != _currentTouches.end())
						it->second = mte->position();
					break;
				}
			}
		}
		else {
			// handle other events here (head tracking?)
		}
	}

	void update(float timeSinceLastUpdate) {
		// we have all current touches and their positions in the _currentTouches
		// and all previous states in _oldTouches.. do something with this!

		// we are only interested on a relative motion of a single touch point
		if (_currentTouches.size() > 0 && _oldTouches.size() > 0) {
			// get the first active touch
			TouchMap::iterator curIt = _currentTouches.begin();

			// look for the same id in the oldTouches map
			TouchMap::iterator oldIt = _oldTouches.find(curIt->first);
			if (oldIt == _oldTouches.end())
				return;

			// now we have found a touch with the same id in the old and in the current
			// map => get its relative translation (in virtual screen coordinates [-1,1] x [-1,1])
			vec3f relMotion = curIt->second - oldIt->second;

			// get the main camera
			Camera& cam = Viargo.camera("main");
			//if (cam == Camera::nil) {
			//	LogError("Something is very wrong.. can't find Viargo's main camera");
			//	return;
			//}

			// unproject a translation in display coordinates to translation in scene coordinates
			vec2f screenMotion = vec2f(2.0f * relMotion.x,  -2.0f * relMotion.y);
			vec3f sceneTranslation = cam.virtualDisplay.unprojectVector(screenMotion);
			
			// finally translate the camera
			//cam.move(sceneTranslation);
			cam.setPosition(cam.position() + sceneTranslation);
			cam.setTarget(cam.target() + sceneTranslation);

			// nothing else to do ...
			// the application's camera will be synchronised with the Viargo's main camera
		}

		// at last - update the _oldTouches map for the next frame
		_oldTouches = _currentTouches;
	}

private:
	typedef std::map<int, vec3f> TouchMap;
	TouchMap _currentTouches; // << current touches (id, 2D position, z = 0)
	TouchMap _oldTouches;     // << old touches (id, 2D position, z = 0)
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Simple metaphor that only moves a registered objects in x/y axes
class ObjectMoveMetaphor : public Metaphor {

public:

	// Constructor
	ObjectMoveMetaphor(std::string name) 
		:Metaphor(name)
	{
	}

	// Destructor
	~ObjectMoveMetaphor() {
	}

	void handleEvent(Event* event) {
		if (typeid(*event) == typeid(RawMultiTouchEvent)) {
			RawMultiTouchEvent* mte = (RawMultiTouchEvent*)event; // it's save to do this :)

			// we are only interested in 2D finger touches!
			if (!mte->is2D() || !mte->isCursor())
				return;

			// only put/update all the touches in the _currentTouches map. 
			// they will by processed in update()
			switch (mte->action()) {
				 // ignore
				case RawMultiTouchEvent::INVALID_ACTION:
					return;
				
				// ignore
				case RawMultiTouchEvent::FRAME:
					return;

				// add new touch
				case RawMultiTouchEvent::APPEARED:{
					_currentTouches.insert(std::make_pair(mte->sessionID(), mte->position()));
					break;
				}
				// remove a touch
				case RawMultiTouchEvent::DISAPPEARED:{
					TouchMap::iterator it = _currentTouches.find(mte->sessionID());
					if (it != _currentTouches.end())
						_currentTouches.erase(it);
					break;
				}
				// update touch position
				case RawMultiTouchEvent::UPDATED:{
					TouchMap::iterator it = _currentTouches.find(mte->sessionID());
					if (it != _currentTouches.end())
						it->second = mte->position();
					break;
				}
			}
		}
		else {
			// handle other events here (head tracking?)
		}
	}

	// resets the "selected" marker for a node in the scene graph
	void resetSelection(SceneNode& node, const mat4f& /*unused*/) {
		if (node.hasProperty("selected"))
			node.setProperty("selected", false);
	}

	void update(float timeSinceLastUpdate) {
		// we have all current touches and their positions in the _currentTouches
		// and all previous states in _oldTouches.. do something with this!

		// reset all "selected" properties
		Viargo.scene().applyOnNode(this, &ObjectMoveMetaphor::resetSelection, true);

		// we are only interested on the relative motion of a single touch point
		if (_currentTouches.size() > 0 && _oldTouches.size() > 0) {
			// get the first active touch
			TouchMap::iterator curIt = _currentTouches.begin();

			// look for the same id in the oldTouches map
			TouchMap::iterator oldIt = _oldTouches.find(curIt->first);
			if (oldIt == _oldTouches.end())
				return;

			// now we have found a touch with the same id in the old and in the current
			// map => get its relative translation (in unified coordinates [0,1] x [0,1])
			vec3f relMotion = curIt->second - oldIt->second;
			vec3f rawTouchPosition = curIt->second;

			// get the viargo camera
			Camera& cam = Viargo.camera("main");

			// recalculate the rawTouchPosition in VirtualScreen coordinates
			vec2f screenPosition(2.0f * rawTouchPosition.x - 1.0f, -(2.0f * rawTouchPosition.y - 1.0f));
			
			// get a selection ray.. ray via the touch point starting at camera's position
			// the ray is in scene coordinates
			ray3f selectionRay = cam.virtualDisplay.cameraRay(screenPosition);
			
			// get all objects hit by the ray
			std::vector<SceneObject> hitObjects = Viargo.scene().select(selectionRay);
			
			// translation vector in virtual screen's coordinates
			vec2 screenMotion(2.0f * relMotion.x, -2.0f * relMotion.y);
			vec3f translation;
			
			for (size_t i = 0; i < hitObjects.size(); ++i) {
					
				// recalculate the relative motion of the touch on the screen as a translation
				// vector in scene coordinates. Pay attention, that the object's translation depends on
				// the object's parallax related to the virtual screen.						  
				translation = cam.virtualDisplay.unprojectVector(screenMotion, 
					cam.virtualDisplay.parallax(hitObjects[i].absolutePosition()));
				// translate the object
				hitObjects[i].translateAbsolute(translation);

				// mark the object as selected
				hitObjects[i].setProperty("selected", true);
			}
		}

		// at last - update the _oldTouches map for the next frame
		_oldTouches = _currentTouches;
	}

private:
	typedef std::map<int, vec3f> TouchMap;
	TouchMap _currentTouches; // << current touches (id, 2D position, z = 0)
	TouchMap _oldTouches;     // << old touches (id, 2D position, z = 0)
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

} // namespace viargo
#endif // MULTI_TOUCH_TEST_METAPHORS_H


