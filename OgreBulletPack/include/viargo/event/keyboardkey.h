#ifndef VIARGO_KEYBOARDKEY_H
#define VIARGO_KEYBOARDKEY_H

namespace viargo {

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class KeyboardKey {

public:
	enum KeyCode {
		// The keyboard syms have been cleverly chosen to map to ASCII
        KEY_FIRST			= 0,
        KEY_UNKNOWN			= 0,
        KEY_BACKSPACE		= 8,
        KEY_TAB				= 9,
        KEY_CLEAR			= 12,
        KEY_ENTER			= 13,
        KEY_PAUSE			= 19,
        KEY_ESCAPE			= 27,
        KEY_SPACE			= 32,
        KEY_EXCLAIM			= 33,
        KEY_QUOTEDBL		= 34,
        KEY_HASH			= 35,
        KEY_DOLLAR			= 36,
        KEY_AMPERSAND		= 38,
        KEY_QUOTE			= 39,
        KEY_LEFTPAREN		= 40,
        KEY_RIGHTPAREN		= 41,
        KEY_ASTERISK		= 42,
        KEY_PLUS			= 43,
        KEY_COMMA			= 44,
        KEY_MINUS			= 45,
        KEY_PERIOD			= 46,
        KEY_SLASH			= 47,
        KEY_0				= 48,
        KEY_1				= 49,
        KEY_2				= 50,
        KEY_3				= 51,
        KEY_4				= 52,
        KEY_5				= 53,
        KEY_6				= 54,
        KEY_7				= 55,
        KEY_8				= 56,
        KEY_9				= 57,
        KEY_COLON			= 58,
        KEY_SEMICOLON		= 59,
        KEY_LESS			= 60,
        KEY_EQUALS			= 61,
        KEY_GREATER			= 62,
        KEY_QUESTION		= 63,
        KEY_AT				= 64,
    
		//Skip uppercase letters
	
        KEY_LEFTBRACKET		= 91,
        KEY_BACKSLASH		= 92,
        KEY_RIGHTBRACKET	= 93,
        KEY_CARET			= 94,
        KEY_UNDERSCORE		= 95,
        KEY_BACKQUOTE		= 96,
        KEY_A				= 97,
        KEY_B				= 98,
        KEY_C				= 99,
        KEY_D				= 100,
        KEY_E				= 101,
        KEY_F				= 102,
        KEY_G				= 103,
        KEY_H				= 104,
        KEY_I				= 105,
        KEY_J				= 106,
        KEY_K				= 107,
        KEY_L				= 108,
        KEY_M				= 109,
        KEY_N				= 110,
        KEY_O				= 111,
        KEY_P				= 112,
        KEY_Q				= 113,
        KEY_R				= 114,
        KEY_S				= 115,
        KEY_T				= 116,
        KEY_U				= 117,
        KEY_V				= 118,
        KEY_W				= 119,
        KEY_X				= 120,
        KEY_Y				= 121,
        KEY_Z				= 122,
        KEY_DELETE			= 127,
        // End of ASCII mapped keysyms
		
        // Numeric keypad
        KEY_KP0				= (0x10000 + KEY_0), //256, ASCII + 0x10000
        KEY_KP1				= (0x10000 + KEY_1), //257,
        KEY_KP2				= (0x10000 + KEY_2), //258,
        KEY_KP3				= (0x10000 + KEY_3), //259,
        KEY_KP4				= (0x10000 + KEY_4), //260,
        KEY_KP5				= (0x10000 + KEY_5), //261,
        KEY_KP6				= (0x10000 + KEY_6), //262,
        KEY_KP7				= (0x10000 + KEY_7), //263,
        KEY_KP8				= (0x10000 + KEY_8), //264,
        KEY_KP9				= (0x10000 + KEY_9), //265,
		KEY_KP_PERIOD		= (0x10000 + KEY_PERIOD), //266,
        KEY_KP_DIVIDE		= (0x10000 + KEY_SLASH),  //267,
        KEY_KP_MULTIPLY		= (0x10000 + KEY_ASTERISK),  //268, 
        KEY_KP_MINUS		= (0x10000 + KEY_MINUS),  //269,
        KEY_KP_PLUS			= (0x10000 + KEY_PLUS),   //270,
        KEY_KP_ENTER		= (0x10000 + KEY_ENTER),  //271,
        KEY_KP_EQUALS		= (0x10000 + KEY_EQUALS), //272,
		
        // Arrows + Home/End pad
        KEY_UP				= (0x20000 + KEY_8), //273,
        KEY_DOWN			= (0x20000 + KEY_2), //274,
        KEY_RIGHT			= (0x20000 + KEY_6), //275,
        KEY_LEFT			= (0x20000 + KEY_4), //276,
        KEY_INSERT			= (0x20000 + KEY_0), //277,
        KEY_HOME			= (0x20000 + KEY_7), //278,
        KEY_END				= (0x20000 + KEY_1), //279,
        KEY_PAGEUP			= (0x20000 + KEY_9), //280,
        KEY_PAGEDOWN		= (0x20000 + KEY_3), //281,
		
        // Function keys
        KEY_F1         = 0x30001, //282,
        KEY_F2         = 0x30002, //283,
        KEY_F3         = 0x30003, //284,
        KEY_F4         = 0x30004, //285,
        KEY_F5         = 0x30005, //286,
        KEY_F6         = 0x30006, //287,
        KEY_F7         = 0x30007, //288,
        KEY_F8         = 0x30008, //289,
        KEY_F9         = 0x30009, //290,
        KEY_F10        = 0x3000A, //291,
        KEY_F11        = 0x3000B, //292,
        KEY_F12        = 0x3000C, //293,
        KEY_F13        = 0x3000D, //294,
        KEY_F14        = 0x3000E, //295,
        KEY_F15        = 0x3000F, //296,
		
        // Key state modifier keys
        KEY_RSHIFT			= 0x40001, // 303,
        KEY_LSHIFT			= 0x40002, // 304,
		KEY_SHIFT			= KEY_RSHIFT | KEY_LSHIFT,

        KEY_RCTRL			= 0x40004, // 305,
        KEY_LCTRL			= 0x40008, // 306,
		KEY_CTRL			= KEY_RCTRL | KEY_LCTRL,

        KEY_RALT			= 0x40010, // 307,
        KEY_LALT			= 0x40020, // 308,
		KEY_ALT				= KEY_RALT | KEY_LALT, 

        KEY_RMETA			= 0x40040, // 309,
        KEY_LMETA			= 0x40080, // 310,
		KEY_META			= KEY_RMETA | KEY_LMETA,

        KEY_RSUPER			= 0x40200, // 312,      /* Right "Windows" key */
        KEY_LSUPER			= 0x40100, // 311,      /* Left "Windows" key */
		KEY_SUPER			= KEY_RSUPER | KEY_LSUPER,
		
		KEY_MODE			= 0x40400, // 313,      /* "Alt Gr" key */
        KEY_COMPOSE			= 0x40800, // 314,      /* Multi-key compose key */
        KEY_NUMLOCK			= 0x41000, // 300,
        KEY_CAPSLOCK		= 0x42000, // 301,
        KEY_SCROLLOCK		= 0x44000, // 302,
		
        // Miscellaneous function keys
        KEY_HELP       = 0x50001, // 315,
        KEY_PRINT      = 0x50002, // 316,
        KEY_SYSREQ     = 0x50003, // 317,
        KEY_BREAK      = 0x50004, // 318,
        KEY_MENU       = 0x50005, // 319,
        KEY_POWER      = 0x50006, // 320,      /* Power Macintosh power key */
        KEY_EURO       = 0x50007, // 321,      /* Some european keyboards */
        KEY_UNDO       = 0x50008, // 322,      /* Atari keyboard has Undo */
	};


	/// Constructor with specified KeyCode
	KeyboardKey(KeyCode key = KEY_UNKNOWN) 
		:_key(key) 
	{	
	}

	/// Constructor from ASCII
	KeyboardKey(char key)
		:_key((KeyCode)(tolower((int)key)))
	{
	}

	/// Destructor
	~KeyboardKey() 
	{
	}

	/// operator ==
	inline bool operator== (const KeyboardKey& key) const {
		return _key == key._key;
	}

	inline bool operator== (KeyCode code) const {
		return _key == code;
	}

	/// operator ==
	// TODO: check for numpad keys
	inline bool operator== (char ascii) const {
		KeyboardKey k(ascii);
		return (*this == k);
	}

	/// operator != 
	inline bool operator!= (const KeyboardKey& key) const {
		return !(*this == key);
	} 

	inline bool operator!= (KeyCode code) const {
		return !(*this == code);
	}

	/// operator != 
	inline bool operator!= (char ascii) const {
		return !(*this == ascii);
	}

	/// assignment operator
	inline KeyboardKey& operator= (KeyCode key) {
		_key = key;
		return *this;
	}

	/// assignment operator
	inline KeyboardKey& operator= (char ascii) {
		KeyboardKey k(ascii);
		_key = k._key;
		return *this;
	}

	/// Returns true if the key has an ASCII mapping, false otherwise
	inline bool isAscii() const {
		return (_key != KEY_UNKNOWN) && (((0xF0000 & _key) == 0) || isNumericPad()) ;
	}
	
	/// Returns true if the key is on the numeric keypad and NumLock is active, false otherwise
	inline bool isNumericPad() const {
		return (_key != KEY_UNKNOWN) && ((0xF0000 & _key) == 1);
	}
	
	/// Returns true if the key is on the numeric keypad and NumLock is inactive, false otherwise
	inline bool isArrowPad() const {
		return (_key != KEY_UNKNOWN) && ((0xF0000 & _key) == 2);
	}
	
	/// Returns true if the key is a function key, false otherwise
	inline bool isFunction() const {
		return (_key != KEY_UNKNOWN) && ((0xF0000 & _key) == 3); 
	}
	
	/// Returns true if the key is a state modifier, false otherwise
	inline bool isStateModifier() const {
		return (_key != KEY_UNKNOWN) && ((0xF0000 & _key) == 4);
	}
	
	/// Returns true if the key is an extra key, false otherwise
	inline bool isExtra() const {
		return (_key != KEY_UNKNOWN) && ((0xF0000 & _key) == 5);
	}

	/// gets the ASCII charecter for the key (if any)
	/// @ returns: ASCII char for the key if the key has an ASCII mapping, -1 otherwise.
	inline char ascii()	const {
		if (isAscii()) 
			return ((char)_key);
		else if (isNumericPad())
			return ((char)(0xFF & _key));
		
		return -1;
	}

	/// Returns the keyCode of this key
	inline KeyCode code()	const {
		return _key;
	}

protected:
	KeyCode _key;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class KeyboardModifier : public KeyboardKey {

public:
	
	/// Constructor from KeyCode. One combine more codes like this: KEY_SHIFT | KEY_ALT | ...
	KeyboardModifier(KeyCode mod) 
		: KeyboardKey(mod)
	{
		if (!isStateModifier())
			_key = KEY_UNKNOWN;
	}
	
	/// Constructor from multiple KeyCodes
	KeyboardModifier(KeyCode mod1, 
					 KeyCode mod2, 
					 KeyCode mod3 = KEY_UNKNOWN, 
					 KeyCode mod4 = KEY_UNKNOWN, 
					 KeyCode mod5 = KEY_UNKNOWN) 
		: KeyboardKey(mod1 | mod2 | mod3 | mod4 | mod5)
	{
		KeyCode code = KEY_UNKNOWN;
		code = (KeyCode)(code | (((0xF0000 & mod1) == 4) ? mod1 : KEY_UNKNOWN ));
		code = (KeyCode)(code | (((0xF0000 & mod2) == 4) ? mod2 : KEY_UNKNOWN ));
		code = (KeyCode)(code | (((0xF0000 & mod3) == 4) ? mod3 : KEY_UNKNOWN ));
		code = (KeyCode)(code | (((0xF0000 & mod4) == 4) ? mod4 : KEY_UNKNOWN ));
		code = (KeyCode)(code | (((0xF0000 & mod5) == 4) ? mod5 : KEY_UNKNOWN ));

		_key = code;
	}

	/// Constructor with a vector of modifier keys
	//KeyboardModifier(std::vector<KeyCode> modifiers) 
	//	: KeyboardKey()
	//{
	//	KeyCode code = KEY_UNKNOWN;
	//	for (size_t i = 0; i < modifiers.size(); ++i)
	//		code = code | modifiers[i];
	//	_key = code;
	//}
	
	/// Constructor with explicit specification of the key state
	KeyboardModifier(bool shift = false,
					 bool ctrl  = false,
					 bool alt   = false,
					 bool meta  = false,
					 bool super = false,	 /// Windows/Mac key
					 bool mode  = false,	 /// Alt-Gr
					 bool compose =	  false, /// Multi-key compose key
					 bool numlock =   false,
					 bool capslock =  false,
					 bool scrollock = false) 
		:KeyboardKey()
	{
		KeyCode code = KEY_UNKNOWN;
		if (shift)		
			code = (KeyCode)(code | KEY_SHIFT);
		
		if (ctrl)
			code = (KeyCode)(code | KEY_CTRL);
		
		if (alt)
			code = (KeyCode)(code | KEY_ALT);
		
		if (meta)
			code = (KeyCode)(code | KEY_META);
		
		if (super)
			code = (KeyCode)(code | KEY_SUPER);
		
		if (mode)
			code = (KeyCode)(code | KEY_MODE);
		
		if (compose)
			code = (KeyCode)(code | KEY_COMPOSE);
		
		if (numlock)
			code = (KeyCode)(code | KEY_NUMLOCK);
		
		if (capslock)
			code = (KeyCode)(code | KEY_CAPSLOCK);
		
		if (scrollock)
			code = (KeyCode)(code | KEY_SCROLLOCK);

		_key = code;
		
	}

	/// Destructor
	~KeyboardModifier() 
	{
	}
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
	
}//namespace viargo
#endif // VIARGO_KEYBOARDKEY_H
