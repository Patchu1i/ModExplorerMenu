
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

	static int VirtualKeyToSkyrim(WPARAM wParam)
	{
		switch (wParam) {
		case VK_TAB:
			return 0x0F;
		case VK_LEFT:
			return 0xCB;
		case VK_RIGHT:
			return 0xCD;
		case VK_UP:
			return 0xC8;
		case VK_DOWN:
			return 0xD0;
		case VK_PRIOR:
			return 0xC9;
		case VK_NEXT:
			return 0xD1;
		case VK_HOME:
			return 0xC7;
		case VK_END:
			return 0xCF;
		case VK_INSERT:
			return 0xD2;
		case VK_DELETE:
			return 0xD3;
		case VK_BACK:
			return 0x0E;
		case VK_SPACE:
			return 0x39;
		case VK_RETURN:
			return 0x1C;
		case VK_ESCAPE:
			return 0x01;
		case VK_OEM_7:
			return 0x28;
		case VK_OEM_COMMA:
			return 0x33;
		case VK_OEM_MINUS:
			return 0x0C;
		case VK_OEM_PERIOD:
			return 0x34;
		case VK_OEM_2:
			return 0x35;
		case VK_OEM_1:
			return 0x27;
		case VK_OEM_PLUS:
			return 0x0D;
		case VK_OEM_4:
			return 0x1A;
		case VK_OEM_5:
			return 0x2B;
		case VK_OEM_6:
			return 0x1B;
		case VK_OEM_3:
			return 0x29;
		case VK_CAPITAL:
			return 0x3A;
		case VK_SCROLL:
			return 0x46;
		case VK_NUMLOCK:
			return 0x45;
		case VK_SNAPSHOT:
			return 0xB7;
		case VK_PAUSE:
			return 0xC5;
		case VK_NUMPAD0:
			return 0x52;
		case VK_NUMPAD1:
			return 0x4F;
		case VK_NUMPAD2:
			return 0x50;
		case VK_NUMPAD3:
			return 0x51;
		case VK_NUMPAD4:
			return 0x4B;
		case VK_NUMPAD5:
			return 0x4C;
		case VK_NUMPAD6:
			return 0x4D;
		case VK_NUMPAD7:
			return 0x47;
		case VK_NUMPAD8:
			return 0x48;
		case VK_NUMPAD9:
			return 0x49;
		case VK_DECIMAL:
			return 0x53;
		case VK_DIVIDE:
			return 0xB5;
		case VK_MULTIPLY:
			return 0x37;
		case VK_SUBTRACT:
			return 0x4A;
		case VK_ADD:
			return 0x4E;
		case VK_RETURN + 256:
			return 0x9C;
		case VK_LSHIFT:
			return 0x2A;
		case VK_LCONTROL:
			return 0x1D;
		case VK_LMENU:
			return 0x38;
		// case VK_LWIN:
		// 	return ImGuiKey_LeftSuper;
		case VK_RSHIFT:
			return 0x36;
		case VK_RCONTROL:
			return 0x9D;
		case VK_RMENU:
			return 0xB8;
		// case VK_RWIN:
		// 	return ImGuiKey_RightSuper;
		// case VK_APPS:
		// 	return ImGuiKey_Menu;
		case '0':
			return 0x0B;
		case '1':
			return 0x02;
		case '2':
			return 0x03;
		case '3':
			return 0x04;
		case '4':
			return 0x05;
		case '5':
			return 0x06;
		case '6':
			return 0x07;
		case '7':
			return 0x08;
		case '8':
			return 0x09;
		case '9':
			return 0x0A;
		case 'A':
			return 0x1E;
		case 'B':
			return 0x30;
		case 'C':
			return 0x2E;
		case 'D':
			return 0x20;
		case 'E':
			return 0x12;
		case 'F':
			return 0x21;
		case 'G':
			return 0x22;
		case 'H':
			return 0x23;
		case 'I':
			return 0x17;
		case 'J':
			return 0x24;
		case 'K':
			return 0x25;
		case 'L':
			return 0x26;
		case 'M':
			return 0x32;
		case 'N':
			return 0x31;
		case 'O':
			return 0x18;
		case 'P':
			return 0x19;
		case 'Q':
			return 0x10;
		case 'R':
			return 0x13;
		case 'S':
			return 0x1F;
		case 'T':
			return 0x14;
		case 'U':
			return 0x16;
		case 'V':
			return 0x2F;
		case 'W':
			return 0x11;
		case 'X':
			return 0x2D;
		case 'Y':
			return 0x15;
		case 'Z':
			return 0x2C;
		case VK_F1:
			return 0x3B;
		case VK_F2:
			return 0x3C;
		case VK_F3:
			return 0x3D;
		case VK_F4:
			return 0x3E;
		case VK_F5:
			return 0x3F;
		case VK_F6:
			return 0x40;
		case VK_F7:
			return 0x41;
		case VK_F8:
			return 0x42;
		case VK_F9:
			return 0x43;
		case VK_F10:
			return 0x44;
		case VK_F11:
			return 0x57;
		case VK_F12:
			return 0x58;
		default:
			return 0x01;
		}
	};

	static const std::unordered_map<int, const char*> ImGuiKeymap = {
		{ 0x0, "Slash" },  // Using "Slash" as "Empty" for ImGui Icons
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
		{ 0x0D, "Equal" },
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
		{ 0x1A, "Bracketleft" },
		{ 0x1B, "Bracketright" },
		{ 0x1C, "Enter" },
		{ 0x1D, "L-Ctrl" },
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
		{ 0x28, "Quotesingle" },
		{ 0x29, "Tilde" },
		{ 0x2A, "L-Shift" },
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
		{ 0x36, "R-Shift" },
		{ 0x37, "NumPadMult" },
		{ 0x38, "L-Alt" },
		{ 0x39, "Space" },
		{ 0x3A, "CapsLock" },
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
		{ 0x45, "NumLock" },
		{ 0x46, "ScrollLock" },
		{ 0x47, "7" },
		{ 0x48, "8" },
		{ 0x49, "9" },
		{ 0x4A, "NumPadMinus" },
		{ 0x4B, "4" },
		{ 0x4C, "5" },
		{ 0x4D, "6" },
		{ 0x4E, "NumPadlus" },
		{ 0x4F, "1" },
		{ 0x50, "2" },
		{ 0x51, "3" },
		{ 0x52, "NumPad0" },
		{ 0x53, "NumPadDec" },
		{ 0x57, "F11" },
		{ 0x58, "F12" },
		{ 0x9D, "R-Ctrl" },
		{ 0xB5, "NumPadDivide" },
		{ 0xB7, "PrintScreen" },
		{ 0xB8, "R-Alt" },
		{ 0xC5, "Pause" },
		{ 0xC7, "Home" },
		{ 0xC8, "Up" },
		{ 0xC9, "PgUp" },
		{ 0xCB, "Left" },
		{ 0xCD, "Right" },
		{ 0xCF, "End" },
		{ 0xD0, "Down" },
		{ 0xD1, "PgDn" },
		{ 0xD2, "Insert" },
		{ 0xD3, "Delete" }
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