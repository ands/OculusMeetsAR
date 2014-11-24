
#ifndef VIARGO_WIIMOTE_EVENT_H
#define VIARGO_WIIMOTE_EVENT_H

#include "core/event.h"
#include "core/vrgmath.h"

namespace viargo {

/**
 * Base struct for Wiimote IR markers. For Viargo it can
 * be seen as a black box for single IR marker information 
 * from a Wiimote
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
struct WiimoteIRMarker {
public:
	// Constructor
	WiimoteIRMarker(bool f = false, float s = 0, vec2f pos = vec2f(0, 0), vec2i raw = vec2i(0, 0))
		:found(f),
		size(s),
		position(pos),
		rawPosition(raw)
	{
	}

	// if the marker is detected
	bool found;
	// the size of the found marker.. currently unsupported
	float size;
	// the position of a marker
	vec2f position;
	// raw position of the marker (in pixel coordinates)
	vec2i rawPosition;
};


/**
 * Base class for Wiimote events. For Viargo it can be seen as a black box 
 * for Wiimote's current state information.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteEvent : public Event {
public:
    enum EventSourceType {
        ES_NUNCHUCK,
        ES_BASE,
		ES_BALANCEBOARD,
		// TODO:dimi: add the other sources (if needed)
    };

	/// Constructor
	/// @param: source	the Wii-Device, that sended this event
	/// @param: id		an unique string, that identifies the connected device
	///
	/// NOTE: If the BalanceBoard is coupled with the Wiimote, there will be 
	/// only a single connection to the host computer. The user should distinguish
	/// between both devices with the "source" parameter in this case.
	WiimoteEvent(std::string device, EventSourceType sourceType, std::string id)
		:Event(device),
		_sourceType(sourceType),
		_sourceId(id)
	{
    }

	// Getter
	EventSourceType sourceType() const { return _sourceType;}
	std::string id() const     {return _sourceId;}

protected:
	EventSourceType _sourceType;
	std::string _sourceId;
};

/**
 * Base class for Wiimote joystick events. For Viargo it can be seen as a black box 
 * for the current position of the Nunch's (or other) Joystick.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteJoystickEvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	the Wii-Device, that sended this event
	/// @param: id		an unique string, that identifies the connected device
	/// @param: value	the position of the joystick [-0.5; 0.5]x[-0.5;0.5]
	/// @param: raw		the raw position of the joystick, as send from the device
    WiimoteJoystickEvent(std::string device, EventSourceType source, std::string id, vec2f value, vec2i raw)
		:WiimoteEvent(device, source, id),
		_value(value),
		_rawValue(raw)
	{
    }

	// Getter
    vec2f value() const    {return _value;}
    vec2i rawValue() const {return _rawValue;}

private:
    vec2f _value;
    vec2i _rawValue;
};

/**
 * Base class for Wiimote button events. For Viargo it can be seen as a black box 
 * for the current button state of the Wiimote.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteButtonEvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	always ES_BASE for this event
	/// @param: id		an unique string, that identifies the connected device
	/// @param: btn1	the binary coded state of A, B, minus, plus, one, two and home buttons
	/// @param: btn2	the binary coded state of up, down, left, and right buttons
    WiimoteButtonEvent(std::string device, EventSourceType source, std::string id, char btn1, char btn2)
		:WiimoteEvent(device, source, id)
	{
		A		= ((btn1 & 0x01) != 0);
		B		= ((btn1 & 0x02) != 0);
		minus	= ((btn1 & 0x04) != 0);
		plus	= ((btn1 & 0x08) != 0);
		one		= ((btn1 & 0x10) != 0);
		two		= ((btn1 & 0x20) != 0);
		home	= ((btn1 & 0x40) != 0);

		up	  = ((btn2 & 0x01) != 0);
		down  = ((btn2 & 0x02) != 0);
		left  = ((btn2 & 0x04) != 0);
		right = ((btn2 & 0x08) != 0);
    }

public:
	bool A, B, minus, plus, one, two, home;
	bool up, down, left, right;
};

/**
 * Base class for Nunch button events. For Viargo it can be seen as a black box 
 * for the current button state of the Nunch.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteNunchButtonEvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	always ES_NUNCHUCK for this event
	/// @param: id		an unique string, that identifies the connected device
	/// @param: btn1	the binary coded state of C and Z buttons
    WiimoteNunchButtonEvent(std::string device, EventSourceType source, std::string id, unsigned char btn)
		:WiimoteEvent(device, source, id)
	{
		C = ((btn & 0x01) != 0);
		Z = ((btn & 0x02) != 0);
    }

public:
	bool C, Z;
};

/**
 * Base class for Wiimote accelerometer events. For Viargo it can be seen as a black box 
 * for the accelerometer state of the Wiimote, Nunch, etc.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteAccelEvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	the Wii-Device, that sended this event
	/// @param: id		an unique string, that identifies the connected device
	/// @param: value	the position of the accelerometer as a normalized 3D vector
	/// @param: raw		the raw position of the accelerometer, as send from the device
    WiimoteAccelEvent(std::string device, EventSourceType source, std::string id, vec3f value, vec3i raw)
		:WiimoteEvent(device, source, id),
		_value(value),
		_rawValue(raw)
	{
    }

	// Getter
    vec3f value() const    {return _value;}
    vec3i rawValue() const {return _rawValue;}

private:
    vec3f _value;
    vec3i _rawValue;
};

/**
 * Base class for Wiimote IR marker events. For Viargo it can be seen as a black box 
 * for information about the currently detected IR marker by the Wiimote's camera.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteIREvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	always ES_BASE for this event
	/// @param: id		an unique string, that identifies the connected device
	/// @param: m		an array of four WiimoteIRMarker structs, providing information
	///					about each of the four marker, that can be tracked. 
	WiimoteIREvent(std::string device, EventSourceType source, std::string id, WiimoteIRMarker m[4])
		:WiimoteEvent(device, source, id)
	{
		memcpy((void*)_marker, (void*)m, sizeof(_marker));
    }

	// Getter
    const WiimoteIRMarker&  marker(int index) const {return _marker[index];}

private:
    WiimoteIRMarker _marker[4];
};

/**
 * Base class for BalanceBoard events. For Viargo it can be seen as a black box 
 * for information about the currently BalanceBoard state.
 * 
 * Notes:
 * - 
 *
 * TODO:
 * - 
 */
class WiimoteBalanceBoardEvent : public WiimoteEvent {
public:
	/// Constructor
	/// @param: source	always ES_BALANCEBOARD for this eventt
	/// @param: id		an unique string, that identifies the connected device
	/// @param: fl		the readings from the front-left sensor
	/// @param: fr		the readings from the front-right sensor
	/// @param: bl		the readings from the bottom-left sensor
	/// @param: br		the readings from the bottom-right sensor
	/// @param: fli		the readings from the front-left sensor as send from the device
	/// @param: fri		the readings from the front-right sensor as send from the device
	/// @param: bli		the readings from the bottom-left sensor as send from the device
	/// @param: bri		the readings from the bottom-right sensor as send from the device
	/// @param: weight	the weight of the user in kg, detected by the BalanceBoard
	/// @param: gravityCenter	the projected center of gravity of the user 
    WiimoteBalanceBoardEvent (std::string device, EventSourceType source, std::string id, float fl, float fr, float bl, float br,
								int fli, int fri, int bli, int bri, float weight, vec2f gravityCenter)
		:WiimoteEvent(device, source, id), 
		_fl(fl), 
		_fr(fr), 
		_bl(bl), 
		_br(br),
		_fli(fli), 
		_fri(fri), 
		_bli(bli), 
		_bri(bri),
		_weight(weight),
		_gravityCenter(gravityCenter)
	{
	}

	// Getter
	float frontLeft()	const	{return _fl;}
	float frontRight()	const	{return _fr;}
	float backLeft()	const	{return _bl;}
	float backRight()	const	{return _br;}

	// Getter
	int rawFrontLeft()	const	{return _fli;}
	int rawFrontRight()	const	{return _fri;}
	int rawBackLeft()	const	{return _bli;}
	int rawBackRight()	const	{return _bri;}

	// Getter
	float weight() const {return _weight;}
	vec2f gravityCenter() const {return _gravityCenter;}

private:
    float _fl;
	float _fr;
	float _bl;
	float _br;
	
	int _fli;
	int _fri;
	int _bli;
	int _bri;
	
	float _weight;
	vec2f _gravityCenter;
};

} // namespace viargo
#endif // VIARGO_WIIMOTE_EVENT_H
