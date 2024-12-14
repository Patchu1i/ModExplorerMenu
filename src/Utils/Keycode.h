
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

		// Mouse Buttons are not included (256 - 265)
	}

	// Windows Virtual Key Codes
	static const int KeyCodes[166] = {
		0x0,   // Undefined
		0x01,  // Left Mouse Button
		0x02,  // Right Mouse Button
		0x03,  // Control-break processing
		0x04,  // Middle Mouse Button
		0x05,  // Mouse 3 Button
		0x06,  // Mouse 4 Button
		0x07,  // Undefined
		0x08,  // BACKSPACE
		0x09,  // TAB
		0x0A,  // Reserved
		0x0B,  // Reserved
		0x0C,  // CLEAR
		0x0D,  // ENTER
		0x0E,  // Undefined
		0x0F,  // Undefined
		0x10,  // SHIFT
		0x11,  // CTRL
		0x12,  // ALT
		0x13,  // PAUSE
		0x14,  // CAPS LOCK
		0x15,  // IME Kana
		0x16,  // Undefined
		0x17,  // IME Junja
		0x18,  // IME Final
		0x19,  // IME Kanji
		0x1A,  // Undefined
		0x1B,  // ESC
		0x1C,  // IME Convert
		0x1D,  // IME Nonconvert
		0x1E,  // IME Accept
		0x1F,  // IME Mode Change Request
		0x20,  // SPACEBAR
		0x21,  // PAGE UP
		0x22,  // PAGE DOWN
		0x23,  // END
		0x24,  // HOME
		0x25,  // LEFT ARROW
		0x26,  // UP ARROW
		0x27,  // RIGHT ARROW
		0x28,  // DOWN ARROW
		0x29,  // SELECT
		0x2A,  // PRINT
		0x2B,  // EXECUTE
		0x2C,  // PRINT SCREEN
		0x2D,  // INSERT
		0x2E,  // DELETE
		0x2F,  // HELP
		0x30,  // 0
		0x31,  // 1
		0x32,  // 2
		0x33,  // 3
		0x34,  // 4
		0x35,  // 5
		0x36,  // 6
		0x37,  // 7
		0x38,  // 8
		0x39,  // 9
		0x3A,  // Undefined
		0x3B,  // Undefined
		0x3C,  // Undefined
		0x3D,  // Undefined
		0x3E,  // Undefined
		0x3F,  // Undefined
		0x40,  // Undefined
		0x41,  // A
		0x42,  // B
		0x43,  // C
		0x44,  // D
		0x45,  // E
		0x46,  // F
		0x47,  // G
		0x48,  // H
		0x49,  // I
		0x4A,  // J
		0x4B,  // K
		0x4C,  // L
		0x4B,  // M
		0x4E,  // N
		0x4F,  // O
		0x50,  // P
		0x51,  // Q
		0x52,  // R
		0x53,  // S
		0x54,  // T
		0x55,  // U
		0x56,  // V
		0x57,  // W
		0x58,  // X
		0x59,  // Y
		0x5A,  // Z
		0x5B,  // Left Windows
		0x5C,  // Right Windows
		0x5D,  // Applications
		0x5E,  // Reserved
		0x5F,  // Computer Sleep
		0x60,  // Numpad1
		0x61,  // Numpad2
		0x62,  // Numpad3
		0x63,  // Numpad4
		0x64,  // Numpad5
		0x65,  // Numpad6
		0x66,  // Numpad7
		0x67,  // Numpad8
		0x68,  // Numpad8
		0x69,  // Numpad9
		0x6A,  // Multiply
		0x6B,  // Add
		0x6C,  // Separator
		0x6D,  // Subtract
		0x6E,  // Decimal
		0x6F,  // Divide
		0x70,  // F1
		0x71,  // F2
		0x72,  // F3
		0x73,  // F4
		0x74,  // F5
		0x75,  // F6
		0x76,  // F7
		0x77,  // F8
		0x78,  // F9
		0x79,  // F10
		0x7A,  // F11
		0x7B,  // F12
		0x7C,  // F13
		0x7D,  // F14
		0x7E,  // F15
		0x7F,  // F16
		0x80,  // F17
		0x81,  // F18
		0x82,  // F19
		0x83,  // F20
		0x84,  // F21
		0x85,  // F22
		0x86,  // F23
		0x87,  // F24
		0x88,  // Unknown
		0x89,  // Unknown
		0x8A,  // Unknown
		0x8B,  // Unknown
		0x8C,  // Unknown
		0x8D,  // Unknown
		0x8E,  // Unknown
		0x8F,  // Unknown
		0x90,  // NUM LOCK
		0x91,  // SCROLL LOCK
		0x92,  // OEM Specific
		0x93,  // OEM Specific
		0x94,  // OEM Specific
		0x95,  // OEM Specific
		0x96,  // OEM Specific
		0x97,  // Unknown
		0x98,  // Unknown
		0x99,  // Unknown
		0x9A,  // Unknown
		0x9B,  // Unknown
		0x9C,  // Unknown
		0x9D,  // Unknown
		0x9E,  // Unknown
		0x9F,  // Unknown
		0xA0,  // Left SHIFT
		0xA1,  // Right SHIFT
		0xA2,  // Left CONTROL
		0xA3,  // Right CONTROL
		0xA4,  // Left MENU
		0xA5   // Right MENU
	};

	// Map of Skyrim Key Codes to Key Names. Use GetSkyrimKeyCode() to get Key.
	static const std::unordered_map<int, const char*> SkyrimKeymap = {
		{ 0x0, "Undefined" },
		{ 0x01, "Escape" },
		{ 0x02, "1" },
		{ 0x03, "2" },
		{ 0x04, "3" },
		{ 0x05, "4" },
		{ 0x06, "5" },
		{ 0x07, "6" },
		{ 0x08, "7" },
		{ 0x09, "8" },
		{ 0x0A, "9" },
		{ 0x0B, "0" },
		{ 0x0C, "Minus" },
		{ 0x0D, "Equals" },
		{ 0x0E, "Backspace" },
		{ 0x0F, "Tab" },
		{ 0x10, "Q" },
		{ 0x11, "W" },
		{ 0x12, "E" },
		{ 0x13, "R" },
		{ 0x14, "T" },
		{ 0x15, "Y" },
		{ 0x16, "U" },
		{ 0x17, "I" },
		{ 0x18, "O" },
		{ 0x19, "P" },
		{ 0x1A, "Left Bracket" },
		{ 0x1B, "Right Bracket" },
		{ 0x1C, "Enter" },
		{ 0x1D, "Left Control" },
		{ 0x1E, "A" },
		{ 0x1F, "S" },
		{ 0x20, "D" },
		{ 0x21, "F" },
		{ 0x22, "G" },
		{ 0x23, "H" },
		{ 0x24, "J" },
		{ 0x25, "K" },
		{ 0x26, "L" },
		{ 0x27, "Semicolon" },
		{ 0x28, "Apostrophe" },
		{ 0x29, "Grave Accent" },
		{ 0x2A, "Left Shift" },
		{ 0x2B, "Backslash" },
		{ 0x2C, "Z" },
		{ 0x2D, "X" },
		{ 0x2E, "C" },
		{ 0x2F, "V" },
		{ 0x30, "B" },
		{ 0x31, "N" },
		{ 0x32, "M" },
		{ 0x33, "Comma" },
		{ 0x34, "Period" },
		{ 0x35, "Slash" },
		{ 0x36, "Right Shift" },
		{ 0x37, "Numpad Asterisk" },
		{ 0x38, "Left Alt" },
		{ 0x39, "Spacebar" },
		{ 0x3A, "Caps Lock" },
		{ 0x3B, "F1" },
		{ 0x3C, "F2" },
		{ 0x3D, "F3" },
		{ 0x3E, "F4" },
		{ 0x3F, "F5" },
		{ 0x40, "F6" },
		{ 0x41, "F7" },
		{ 0x42, "F8" },
		{ 0x43, "F9" },
		{ 0x44, "F10" },
		{ 0x45, "Num Lock" },
		{ 0x46, "Scroll Lock" },
		{ 0x47, "Numpad 7" },
		{ 0x48, "Numpad 8" },
		{ 0x49, "Numpad 9" },
		{ 0x4A, "Numpad Minus" },
		{ 0x4B, "Numpad 4" },
		{ 0x4C, "Numpad 5" },
		{ 0x4D, "Numpad 6" },
		{ 0x4E, "Numpad Plus" },
		{ 0x4F, "Numpad 1" },
		{ 0x50, "Numpad 2" },
		{ 0x51, "Numpad 3" },
		{ 0x52, "Numpad 0" },
		{ 0x53, "Numpad Period" },
		{ 0x57, "F11" },
		{ 0x58, "F12" },
		{ 0x9D, "Right Control" },
		{ 0xB5, "Numpad Divide" },
		{ 0xB7, "Print Screen" },
		{ 0xB8, "Right Alt" },
		{ 0xC5, "Pause" },
		{ 0xC7, "Home" },
		{ 0xC8, "Up Arrow" },
		{ 0xC9, "Page Up" },
		{ 0xCB, "Left Arrow" },
		{ 0xCD, "Right Arrow" },
		{ 0xCF, "End" },
		{ 0xD0, "Down Arrow" },
		{ 0xD1, "Page Down" },
		{ 0xD2, "Insert" },
		{ 0xD3, "Delete" }
	};
}  // namespace ImGui