
namespace ImGui
{
	static bool IsKeyboardWhitelist(int key)
	{
		return (key != VK_LBUTTON &&     // Left mouse button
				key != VK_RBUTTON &&     // Right mouse button
				key != VK_MBUTTON &&     // Middle mouse button
				key != VK_XBUTTON1 &&    // Mouse 3 button
				key != VK_XBUTTON2 &&    // Mouse 4 button
				key != VK_CANCEL &&      // Control-break processing
				key != VK_ZOOM &&        // Reserved
				key != VK_BACK &&        // BACKSPACE key
				key != VK_TAB &&         // TAB key
				key != VK_CLEAR &&       // CLEAR key
				key != VK_RETURN &&      // ENTER key
				key != VK_SHIFT &&       // SHIFT key
				key != VK_LSHIFT &&      // Left SHIFT key
				key != VK_RSHIFT &&      // Right SHIFT key
				key != VK_CONTROL &&     // CTRL key
				key != VK_LCONTROL &&    // Left CTRL key
				key != VK_RCONTROL &&    // Right CTRL key
				key != VK_LMENU &&       // Left ALT key
				key != VK_RMENU &&       // Right ALT key
				key != VK_MENU &&        // ALT key
				key != VK_CAPITAL &&     // CAPS LOCK key
				key != VK_ESCAPE &&      // ESC key
				key != VK_MODECHANGE &&  // MODE CHANGE key
				key != VK_SPACE);        // SPACEBAR
	}

	// GetAsyncKeyState() distinguishes between VK_L and VK_R
	static bool IsModifierKey(int key)
	{
		return (key == VK_SHIFT ||     // SHIFT key
				key == VK_LSHIFT ||    // Left SHIFT key
				key == VK_RSHIFT ||    // Right SHIFT key
				key == VK_CONTROL ||   // CTRL key
				key == VK_LCONTROL ||  // Left CTRL key
				key == VK_RCONTROL ||  // Right CTRL key
				key == VK_MENU ||      // ALT key
				key == VK_LMENU ||     // Left ALT key
				key == VK_RMENU);      // Right ALT key
	}

	// Only allows for few mouse buttons.
	// static int GetSkyrimMouseCode(int key)
	// {
	// 	switch (key) {
	// 	case VK_LBUTTON:
	// 		return 0x100;  // MOUSE1
	// 	case VK_RBUTTON:
	// 		return 0x101;  // MOUSE2
	// 	case VK_MBUTTON:
	// 		return 0x102;  // MOUSE3
	// 	case VK_XBUTTON1:
	// 		return 0x103;  // MOUSE4
	// 	case VK_XBUTTON2:
	// 		return 0x104;  // MOUSE5
	// 	default:
	// 		return 0;
	// 	}
	// }

	// Translate Win Virtual Key Codes to Skyrim Key Codes (Keyboard Only)
	// Missing some Numpad (+ and -) and some function keys!
	static int GetSkyrimKeyCode(int key)
	{
		switch (key) {
		case 0x30:
			return 0x0B;  // 0
		case 0x31:
			return 0x02;  // 1
		case 0x32:
			return 0x03;  // 2
		case 0x33:
			return 0x04;  // 3
		case 0x34:
			return 0x05;  // 4
		case 0x35:
			return 0x06;  // 5
		case 0x36:
			return 0x07;  // 6
		case 0x37:
			return 0x08;  // 7
		case 0x38:
			return 0x09;  // 8
		case 0x39:
			return 0x0A;  // 9
		case VK_SUBTRACT:
			return 0x0C;  // MINUS / UNDERSCORE
		case VK_ADD:
			return 0x0D;  // EQUALS / ADD
		case VK_BACK:
			return 0x0E;  // BACKSPACE
		case VK_TAB:
			return 0x0F;  // TAB
		case 0x51:
			return 0x10;  // Q
		case 0x57:
			return 0x11;  // W
		case 0x45:
			return 0x12;  // E
		case 0x52:
			return 0x13;  // R
		case 0x54:
			return 0x14;  // T
		case 0x59:
			return 0x15;  // Y
		case 0x55:
			return 0x16;  // U
		case 0x49:
			return 0x17;  // I
		case 0x4F:
			return 0x18;  // O
		case 0x50:
			return 0x19;  // P
		case VK_OEM_4:
			return 0x1A;  // LEFT BRACKET / LEFT CURLY BRACE
		case VK_OEM_6:
			return 0x1B;  // RIGHT BRACKET / RIGHT CURLY BRACE
		case VK_RETURN:
			return 0x1C;  // ENTER
		case VK_LCONTROL:
			return 0x1D;  // LEFT CONTROL
		case 0x41:
			return 0x1E;  // A
		case 0x53:
			return 0x1F;  // S
		case 0x44:
			return 0x20;  // D
		case 0x46:
			return 0x21;  // F
		case 0x47:
			return 0x22;  // G
		case 0x48:
			return 0x23;  // H
		case 0x4A:
			return 0x24;  // J
		case 0x4B:
			return 0x25;  // K
		case 0x4C:
			return 0x26;  // L
		case VK_OEM_1:
			return 0x27;  // SEMICOLON / COLON
		case VK_OEM_7:
			return 0x28;  // APOSTROPHE / DOUBLE QUOTE
		case VK_OEM_3:
			return 0x29;  // GRAVE ACCENT / TILDE
		case VK_LSHIFT:
			return 0x2A;  // LEFT SHIFT
		case VK_OEM_5:
			return 0x2B;  // BACKSLASH / PIPE
		case 0x5A:
			return 0x2C;  // Z
		case 0x58:
			return 0x2D;  // X
		case 0x43:
			return 0x2E;  // C
		case 0x56:
			return 0x2F;  // V
		case 0x42:
			return 0x30;  // B
		case 0x4E:
			return 0x31;  // N
		case 0x4D:
			return 0x32;  // M
		case VK_OEM_COMMA:
			return 0x33;  // COMMA / LESS THAN
		case VK_OEM_PERIOD:
			return 0x34;  // PERIOD / GREATER THAN
		case VK_OEM_2:
			return 0x35;  // SLASH / QUESTION MARK
		case VK_RSHIFT:
			return 0x36;  // RIGHT SHIFT
		case VK_MULTIPLY:
			return 0x37;  // NUMPAD ASTERISK
		case VK_LMENU:
			return 0x38;  // LEFT ALT
		case VK_SPACE:
			return 0x39;  // SPACEBAR
		case VK_CAPITAL:
			return 0x3A;  // CAPS LOCK
		case VK_F1:
			return 0x3B;  // F1
		case VK_F2:
			return 0x3C;  // F2
		case VK_F3:
			return 0x3D;  // F3
		case VK_F4:
			return 0x3E;  // F4
		case VK_F5:
			return 0x3F;  // F5
		case VK_F6:
			return 0x40;  // F6
		case VK_F7:
			return 0x41;  // F7
		case VK_F8:
			return 0x42;  // F8
		case VK_F9:
			return 0x43;  // F9
		case VK_F10:
			return 0x44;  // F10
		case VK_NUMLOCK:
			return 0x45;  // NUM LOCK
		case VK_SCROLL:
			return 0x46;  // SCROLL LOCK
		case VK_NUMPAD7:
			return 0x47;  // NUMPAD 7
		case VK_NUMPAD8:
			return 0x48;  // NUMPAD 8
		case VK_NUMPAD9:
			return 0x49;  // NUMPAD 9
		//case VK_SUBTRACT:
		//    return 0x4A; // NUMPAD MINUS
		case VK_NUMPAD4:
			return 0x4B;  // NUMPAD 4
		case VK_NUMPAD5:
			return 0x4C;  // NUMPAD 5
		case VK_NUMPAD6:
			return 0x4D;  // NUMPAD 6
		// case VK_ADD:
		//     return 0x4E; // NUMPAD PLUS
		case VK_NUMPAD1:
			return 0x4F;  // NUMPAD 1
		case VK_NUMPAD2:
			return 0x50;  // NUMPAD 2
		case VK_NUMPAD3:
			return 0x51;  // NUMPAD 3
		case VK_NUMPAD0:
			return 0x52;  // NUMPAD 0
		case VK_DECIMAL:
			return 0x53;  // NUMPAD PERIOD
		case VK_F11:
			return 0x57;  // F11
		case VK_F12:
			return 0x58;  // F12
		case VK_RCONTROL:
			return 0x9D;  // RIGHT CONTROL
		case VK_DIVIDE:
			return 0xB5;  // NUMPAD DIVIDE
		case VK_SNAPSHOT:
			return 0xB7;  // PRINT SCREEN
		case VK_RMENU:
			return 0xB8;  // RIGHT ALT
		case VK_PAUSE:
			return 0xC5;  // PAUSE
		case VK_HOME:
			return 0xC7;  // HOME
		case VK_UP:
			return 0xC8;  // UP ARROW
		case VK_PRIOR:
			return 0xC9;  // PAGE UP
		case VK_LEFT:
			return 0xCB;  // LEFT ARROW
		case VK_RIGHT:
			return 0xCD;  // RIGHT ARROW
		case VK_END:
			return 0xCF;  // END
		case VK_DOWN:
			return 0xD0;  // DOWN ARROW
		case VK_NEXT:
			return 0xD1;  // PAGE DOWN
		case VK_INSERT:
			return 0xD2;  // INSERT
		case VK_DELETE:
			return 0xD3;  // DELETE
		default:
			return 0;
		}
	}

	static const char* KeyNames[] = {
		"OFF",
		"VK_LBUTTON",
		"VK_RBUTTON",
		"VK_CANCEL",
		"VK_MBUTTON",
		"VK_XBUTTON1",
		"VK_XBUTTON2",
		"Unknown",
		"VK_BACK",
		"VK_TAB",
		"Unknown",
		"Unknown",
		"VK_CLEAR",
		"VK_RETURN",
		"Unknown",
		"Unknown",
		"VK_SHIFT",
		"VK_CONTROL",
		"VK_MENU",
		"VK_PAUSE",
		"VK_CAPITAL",
		"VK_KANA",
		"Unknown",
		"VK_JUNJA",
		"VK_FINAL",
		"VK_KANJI",
		"Unknown",
		"VK_ESCAPE",
		"VK_CONVERT",
		"VK_NONCONVERT",
		"VK_ACCEPT",
		"VK_MODECHANGE",
		"VK_SPACE",
		"VK_PRIOR",
		"VK_NEXT",
		"VK_END",
		"VK_HOME",
		"VK_LEFT",
		"VK_UP",
		"VK_RIGHT",
		"VK_DOWN",
		"VK_SELECT",
		"VK_PRINT",
		"VK_EXECUTE",
		"VK_SNAPSHOT",
		"VK_INSERT",
		"VK_DELETE",
		"VK_HELP",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"VK_LWIN",
		"VK_RWIN",
		"VK_APPS",
		"Unknown",
		"VK_SLEEP",
		"VK_NUMPAD0",
		"VK_NUMPAD1",
		"VK_NUMPAD2",
		"VK_NUMPAD3",
		"VK_NUMPAD4",
		"VK_NUMPAD5",
		"VK_NUMPAD6",
		"VK_NUMPAD7",
		"VK_NUMPAD8",
		"VK_NUMPAD9",
		"VK_MULTIPLY",
		"VK_ADD",
		"VK_SEPARATOR",
		"VK_SUBTRACT",
		"VK_DECIMAL",
		"VK_DIVIDE",
		"VK_F1",
		"VK_F2",
		"VK_F3",
		"VK_F4",
		"VK_F5",
		"VK_F6",
		"VK_F7",
		"VK_F8",
		"VK_F9",
		"VK_F10",
		"VK_F11",
		"VK_F12",
		"VK_F13",
		"VK_F14",
		"VK_F15",
		"VK_F16",
		"VK_F17",
		"VK_F18",
		"VK_F19",
		"VK_F20",
		"VK_F21",
		"VK_F22",
		"VK_F23",
		"VK_F24",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"VK_NUMLOCK",
		"VK_SCROLL",
		"VK_OEM_NEC_EQUAL",
		"VK_OEM_FJ_MASSHOU",
		"VK_OEM_FJ_TOUROKU",
		"VK_OEM_FJ_LOYA",
		"VK_OEM_FJ_ROYA",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"VK_LSHIFT",
		"VK_RSHIFT",
		"VK_LCONTROL",
		"VK_RCONTROL",
		"VK_LMENU",
		"VK_RMENU"
	};
	static const int KeyCodes[] = {
		0x0,  //Undefined
		0x01,
		0x02,
		0x03,
		0x04,
		0x05,
		0x06,
		0x07,  //Undefined
		0x08,
		0x09,
		0x0A,  //Reserved
		0x0B,  //Reserved
		0x0C,
		0x0D,
		0x0E,  //Undefined
		0x0F,  //Undefined
		0x10,
		0x11,
		0x12,
		0x13,
		0x14,
		0x15,
		0x16,  //IME On
		0x17,
		0x18,
		0x19,
		0x1A,  //IME Off
		0x1B,
		0x1C,
		0x1D,
		0x1E,
		0x1F,
		0x20,
		0x21,
		0x22,
		0x23,
		0x24,
		0x25,
		0x26,
		0x27,
		0x28,
		0x29,
		0x2A,
		0x2B,
		0x2C,
		0x2D,
		0x2E,
		0x2F,
		0x30,
		0x31,
		0x32,
		0x33,
		0x34,
		0x35,
		0x36,
		0x37,
		0x38,
		0x39,
		0x3A,  //Undefined
		0x3B,  //Undefined
		0x3C,  //Undefined
		0x3D,  //Undefined
		0x3E,  //Undefined
		0x3F,  //Undefined
		0x40,  //Undefined
		0x41,
		0x42,
		0x43,
		0x44,
		0x45,
		0x46,
		0x47,
		0x48,
		0x49,
		0x4A,
		0x4B,
		0x4C,
		0x4B,
		0x4E,
		0x4F,
		0x50,
		0x51,
		0x52,
		0x53,
		0x54,
		0x55,
		0x56,
		0x57,
		0x58,
		0x59,
		0x5A,
		0x5B,
		0x5C,
		0x5D,
		0x5E,  //Rservered
		0x5F,
		0x60,  //Numpad1
		0x61,  //Numpad2
		0x62,  //Numpad3
		0x63,  //Numpad4
		0x64,  //Numpad5
		0x65,  //Numpad6
		0x66,  //Numpad7
		0x67,  //Numpad8
		0x68,  //Numpad8
		0x69,  //Numpad9
		0x6A,
		0x6B,
		0x6C,
		0x6D,
		0x6E,
		0x6F,
		0x70,  //F1
		0x71,  //F2
		0x72,  //F3
		0x73,  //F4
		0x74,  //F5
		0x75,  //F6
		0x76,  //F7
		0x77,  //F8
		0x78,  //F9
		0x79,  //F10
		0x7A,  //F11
		0x7B,  //F12
		0x7C,  //F13
		0x7D,  //F14
		0x7E,  //F15
		0x7F,  //F16
		0x80,  //F17
		0x81,  //F18
		0x82,  //F19
		0x83,  //F20
		0x84,  //F21
		0x85,  //F22
		0x86,  //F23
		0x87,  //F24
		0x88,  //Unkown
		0x89,  //Unkown
		0x8A,  //Unkown
		0x8B,  //Unkown
		0x8C,  //Unkown
		0x8D,  //Unkown
		0x8E,  //Unkown
		0x8F,  //Unkown
		0x90,
		0x91,
		0x92,  //OEM Specific
		0x93,  //OEM Specific
		0x94,  //OEM Specific
		0x95,  //OEM Specific
		0x96,  //OEM Specific
		0x97,  //Unkown
		0x98,  //Unkown
		0x99,  //Unkown
		0x9A,  //Unkown
		0x9B,  //Unkown
		0x9C,  //Unkown
		0x9D,  //Unkown
		0x9E,  //Unkown
		0x9F,  //Unkown
		0xA0,
		0xA1,
		0xA2,
		0xA3,
		0xA4,
		0xA5
	};
}  // namespace ImGui