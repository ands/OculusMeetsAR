#ifndef VIARGO_KEY_EVENT_H
#define VIARGO_KEY_EVENT_H

#include "core/event.h"
#include "event/keyboardkey.h"

namespace viargo {

/**
 * Base class for key events.
 *
 */

class KeyEvent : public Event {
	
public:

	enum KeyEventAction {
		KEY_PRESSED,
		KEY_RELEASED
	};

	KeyEvent(std::string device, KeyEventAction action, KeyboardKey key, KeyboardModifier modifier = KeyboardModifier()) 
		:Event(device), 
		_key(key.code()),
		_modifier(modifier.code()),
		_action(action)
	{
	}
	
	KeyEvent(std::string device, KeyEventAction action, char ascii, KeyboardModifier modifier = KeyboardModifier())
		:Event(device), 
		_key(ascii),
		_modifier(modifier.code()),
		_action(action)
	{
	}
	
	~KeyEvent() 
	{
	}
    
	KeyboardKey key() const {
		return _key;
	}
    
	KeyboardModifier modifier() const {
		return _modifier;
	}

	KeyEventAction action() const {
		return _action;
	}
	
private:
    KeyboardKey		 _key;
    KeyboardModifier _modifier;
	KeyEventAction   _action;
};
	
}//namespace viargo
#endif // VIARGO_KEY_EVENT_H
