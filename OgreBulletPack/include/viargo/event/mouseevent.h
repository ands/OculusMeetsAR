#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "core/event.h"
#include "core/vrgmath.h"

namespace viargo{
	/* Base class for mouse events.
	 *
	 *
	 */
	
	class MouseEvent : public Event {
		
	public:
		enum MouseButtons {
			MOUSE_BUTTON_NONE =     0,
			MOUSE_BUTTON_1 =   1 << 0, MOUSE_BUTTON_LEFT   = MOUSE_BUTTON_1,
			MOUSE_BUTTON_2 =   1 << 1, MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_2,
			MOUSE_BUTTON_3 =   1 << 2, MOUSE_BUTTON_RIGHT  = MOUSE_BUTTON_3,
			MOUSE_BUTTON_4 =   1 << 3,
			MOUSE_BUTTON_5 =   1 << 4,
			MOUSE_WHEEL_DOWN = 1 << 5,
			MOUSE_WHEEL_UP =   1 << 6,
			MOUSE_WHEEL    = MOUSE_WHEEL_DOWN | MOUSE_WHEEL_UP,
			MOUSE_BUTTON_ALL = MOUSE_BUTTON_1 | MOUSE_BUTTON_2 | MOUSE_BUTTON_3 | MOUSE_BUTTON_4 | MOUSE_BUTTON_5 | MOUSE_WHEEL
		};
		
		enum MouseAction {
			ACTION_NONE = 0,
			MOTION      = 1 << 0,
			PRESSED     = 1 << 1,
			RELEASED    = 1 << 2, CLICK = PRESSED | RELEASED,
			DOUBLECLICK = 1 << 3,
			WHEEL       = 1 << 4,
			ENTER       = 1 << 5,
			EXIT        = 1 << 6, ENTER_EXIT = ENTER | EXIT,
			ACTION_ALL  = CLICK | MOTION | DOUBLECLICK | WHEEL | ENTER_EXIT
		};
		
		enum Modifier {
			MODIFIER_NONE   = 0x0000,
			SHIFT           = 0x0001,
			CTRL            = 0x0002,
			ALT             = 0x0004,
			META            = 0x0008,
			NUM             = 0x0010,
			CAPS            = 0x0020,
			MODE            = 0x0040,
			RESERVED        = 0x8000,
		};
		
		enum eventType {
			MOUSEPRESSEVENT       = 0x0001,
			MOUSERELEASEEVENT     = 0x0002,
			MOUSEMOVEEVENT        = 0x0004,
			MOUSEDOUBLECLICKEVENT = 0x0008,
			MOUSEENTEREVENT       = 0x0010,
			MOUSEEXITEVENT        = 0x0020,
			WHEELEVENT            = 0x0040,
			TIMEREVENT            = 0x0080,
			KEYEVENT              = 0x0100,
			ROTATIONEVENT         = 0x0200,
			ZOOMEVENT             = 0x0400,
			SIZINGEVENT           = 0x0800,
			TRANSLATIONEVENT      = 0x1000,
			
			ALLEVENTTYPES = MOUSEPRESSEVENT | MOUSERELEASEEVENT | MOUSEMOVEEVENT | MOUSEDOUBLECLICKEVENT | MOUSEENTEREVENT | MOUSEEXITEVENT | WHEELEVENT | TIMEREVENT | KEYEVENT | ROTATIONEVENT | ZOOMEVENT | SIZINGEVENT | TRANSLATIONEVENT
		};
		
		//Constructor
		MouseEvent(std::string device, float x, float y, float z, MouseAction action, MouseButtons button);
		MouseEvent(std::string device, float x, float y, float z, MouseAction action, MouseButtons button, Modifier mod);
		
		//Getter
		vec3 coord() const;
		float x() const;
		float y() const;
		float z() const;
		MouseButtons button() const;
		MouseAction action() const;
		MouseEvent::Modifier modifiers() const;
		virtual int getEventType();
		
	private:
		vec3 _coord;
		MouseAction _action;
		MouseButtons _buttonCode;
		MouseEvent::Modifier _mod;
		
	};
}//namespace viargo
#endif