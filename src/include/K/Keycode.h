
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)

// TODO: ImGui Impl has its own functions for Virtual key to ImGui key conversions.

namespace ImGui
{
	// Used for conditional hotkey assignment. Prevent users from assigning invalid keys to hotkey.
	static inline bool IsKeyboardWhitelist(ImGuiKey a_key)
	{
		return (a_key != ImGuiKey_MouseLeft &&    // Left mouse button
				a_key != ImGuiKey_MouseRight &&   // Right mouse button
				a_key != ImGuiKey_MouseMiddle &&  // Middle mouse button
				a_key != ImGuiKey_MouseX1 &&      // Mouse 3 button
				a_key != ImGuiKey_MouseX2 &&      // Mouse 4 button
				a_key != ImGuiKey_Enter &&        // ENTER key
				a_key != ImGuiKey_Tab &&          // TAB key
				a_key != ImGuiKey_CapsLock &&     // Left ALT key
				a_key != ImGuiKey_Escape &&       // Right ALT key
				a_key != ImGuiKey_T &&            // T key (default)
				a_key != ImGuiKey_Space           // ALT key
		);
	}

	static inline bool IsKeyboardModifier(ImGuiKey a_key)
	{
		return (a_key == ImGuiKey_LeftShift ||   // Left Shift key
				a_key == ImGuiKey_RightShift ||  // Right Shift key
				a_key == ImGuiKey_LeftCtrl ||    // Left Control key
				a_key == ImGuiKey_RightCtrl ||   // Right Control key
				a_key == ImGuiKey_LeftAlt ||     // Left Alt key
				a_key == ImGuiKey_RightAlt       // Right Alt key
		);
	}

	static inline uint32_t ImGuiKeyToSkyrimKey(ImGuiKey a_key)
	{
		switch (a_key) {
		case ImGuiKey_Tab:
			return 0x0F;
		case ImGuiKey_LeftArrow:
			return 0xCB;
		case ImGuiKey_RightArrow:
			return 0xCD;
		case ImGuiKey_UpArrow:
			return 0xC8;
		case ImGuiKey_DownArrow:
			return 0xD0;
		case ImGuiKey_PageUp:
			return 0xC9;
		case ImGuiKey_PageDown:
			return 0xD1;
		case ImGuiKey_Home:
			return 0xC7;
		case ImGuiKey_End:
			return 0xCF;
		case ImGuiKey_Insert:
			return 0xD2;
		case ImGuiKey_Delete:
			return 0xD3;
		case ImGuiKey_Backspace:
			return 0x0E;
		case ImGuiKey_Space:
			return 0x39;
		case ImGuiKey_Enter:
			return 0x1C;
		case ImGuiKey_Escape:
			return 0x01;
		case ImGuiKey_Apostrophe:
			return 0x28;
		case ImGuiKey_Comma:
			return 0x33;
		case ImGuiKey_Minus:
			return 0x0C;
		case ImGuiKey_Period:
			return 0x34;
		case ImGuiKey_Slash:
			return 0x35;
		case ImGuiKey_Semicolon:
			return 0x27;
		case ImGuiKey_Equal:
			return 0x0D;
		case ImGuiKey_LeftBracket:
			return 0x1A;
		case ImGuiKey_Backslash:
			return 0x2B;
		case ImGuiKey_RightBracket:
			return 0x1B;
		case ImGuiKey_GraveAccent:
			return 0x29;
		case ImGuiKey_CapsLock:
			return 0x3A;
		case ImGuiKey_ScrollLock:
			return 0x46;
		case ImGuiKey_NumLock:
			return 0x45;
		case ImGuiKey_PrintScreen:
			return 0xB7;
		case ImGuiKey_Pause:
			return 0xC5;
		case ImGuiKey_Keypad0:
			return 0x52;
		case ImGuiKey_Keypad1:
			return 0x4F;
		case ImGuiKey_Keypad2:
			return 0x50;
		case ImGuiKey_Keypad3:
			return 0x51;
		case ImGuiKey_Keypad4:
			return 0x4B;
		case ImGuiKey_Keypad5:
			return 0x4C;
		case ImGuiKey_Keypad6:
			return 0x4D;
		case ImGuiKey_Keypad7:
			return 0x47;
		case ImGuiKey_Keypad8:
			return 0x48;
		case ImGuiKey_Keypad9:
			return 0x49;
		case ImGuiKey_KeypadDecimal:
			return 0x53;
		case ImGuiKey_KeypadDivide:
			return 0xB5;
		case ImGuiKey_KeypadMultiply:
			return 0x37;
		case ImGuiKey_KeypadSubtract:
			return 0x4A;
		case ImGuiKey_KeypadAdd:
			return 0x4E;
		case ImGuiKey_KeypadEnter:
			return 0x9C;
		case ImGuiKey_LeftShift:
			return 0x2A;
		case ImGuiKey_LeftCtrl:
			return 0x1D;
		case ImGuiKey_LeftAlt:
			return 0x38;
		case ImGuiKey_LeftSuper:
			return 0x5B;
		case ImGuiKey_RightShift:
			return 0x36;
		case ImGuiKey_RightCtrl:
			return 0x9D;
		case ImGuiKey_RightAlt:
			return 0xB8;
		case ImGuiKey_RightSuper:
			return 0x5C;
		case ImGuiKey_Menu:
			return 0x5D;
		case ImGuiKey_0:
			return 0x0B;
		case ImGuiKey_1:
			return 0x02;
		case ImGuiKey_2:
			return 0x03;
		case ImGuiKey_3:
			return 0x04;
		case ImGuiKey_4:
			return 0x05;
		case ImGuiKey_5:
			return 0x06;
		case ImGuiKey_6:
			return 0x07;
		case ImGuiKey_7:
			return 0x08;
		case ImGuiKey_8:
			return 0x09;
		case ImGuiKey_9:
			return 0x0A;
		case ImGuiKey_A:
			return 0x1E;
		case ImGuiKey_B:
			return 0x30;
		case ImGuiKey_C:
			return 0x2E;
		case ImGuiKey_D:
			return 0x20;
		case ImGuiKey_E:
			return 0x12;
		case ImGuiKey_F:
			return 0x21;
		case ImGuiKey_G:
			return 0x22;
		case ImGuiKey_H:
			return 0x23;
		case ImGuiKey_I:
			return 0x17;
		case ImGuiKey_J:
			return 0x24;
		case ImGuiKey_K:
			return 0x25;
		case ImGuiKey_L:
			return 0x26;
		case ImGuiKey_M:
			return 0x32;
		case ImGuiKey_N:
			return 0x31;
		case ImGuiKey_O:
			return 0x18;
		case ImGuiKey_P:
			return 0x19;
		case ImGuiKey_Q:
			return 0x10;
		case ImGuiKey_R:
			return 0x13;
		case ImGuiKey_S:
			return 0x1F;
		case ImGuiKey_T:
			return 0x14;
		case ImGuiKey_U:
			return 0x16;
		case ImGuiKey_V:
			return 0x2F;
		case ImGuiKey_W:
			return 0x11;
		case ImGuiKey_X:
			return 0x2D;
		case ImGuiKey_Y:
			return 0x15;
		case ImGuiKey_Z:
			return 0x2C;
		case ImGuiKey_F1:
			return 0x3B;
		case ImGuiKey_F2:
			return 0x3C;
		case ImGuiKey_F3:
			return 0x3D;
		case ImGuiKey_F4:
			return 0x3E;
		case ImGuiKey_F5:
			return 0x3F;
		case ImGuiKey_F6:
			return 0x40;
		case ImGuiKey_F7:
			return 0x41;
		case ImGuiKey_F8:
			return 0x42;
		case ImGuiKey_F9:
			return 0x43;
		case ImGuiKey_F10:
			return 0x44;
		case ImGuiKey_F11:
			return 0x57;
		case ImGuiKey_F12:
			return 0x58;
		default:
			return 0;
		}
	}

	// Translates Virtual Key Codes into ImGui compatible codes for ImGui specific Input.
	static inline ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam)
	{
		switch (wParam) {
		case VK_TAB:
			return ImGuiKey_Tab;
		case VK_LEFT:
			return ImGuiKey_LeftArrow;
		case VK_RIGHT:
			return ImGuiKey_RightArrow;
		case VK_UP:
			return ImGuiKey_UpArrow;
		case VK_DOWN:
			return ImGuiKey_DownArrow;
		case VK_PRIOR:
			return ImGuiKey_PageUp;
		case VK_NEXT:
			return ImGuiKey_PageDown;
		case VK_HOME:
			return ImGuiKey_Home;
		case VK_END:
			return ImGuiKey_End;
		case VK_INSERT:
			return ImGuiKey_Insert;
		case VK_DELETE:
			return ImGuiKey_Delete;
		case VK_BACK:
			return ImGuiKey_Backspace;
		case VK_SPACE:
			return ImGuiKey_Space;
		case VK_RETURN:
			return ImGuiKey_Enter;
		case VK_ESCAPE:
			return ImGuiKey_Escape;
		case VK_OEM_7:
			return ImGuiKey_Apostrophe;
		case VK_OEM_COMMA:
			return ImGuiKey_Comma;
		case VK_OEM_MINUS:
			return ImGuiKey_Minus;
		case VK_OEM_PERIOD:
			return ImGuiKey_Period;
		case VK_OEM_2:
			return ImGuiKey_Slash;
		case VK_OEM_1:
			return ImGuiKey_Semicolon;
		case VK_OEM_PLUS:
			return ImGuiKey_Equal;
		case VK_OEM_4:
			return ImGuiKey_LeftBracket;
		case VK_OEM_5:
			return ImGuiKey_Backslash;
		case VK_OEM_6:
			return ImGuiKey_RightBracket;
		case VK_OEM_3:
			return ImGuiKey_GraveAccent;
		case VK_CAPITAL:
			return ImGuiKey_CapsLock;
		case VK_SCROLL:
			return ImGuiKey_ScrollLock;
		case VK_NUMLOCK:
			return ImGuiKey_NumLock;
		case VK_SNAPSHOT:
			return ImGuiKey_PrintScreen;
		case VK_PAUSE:
			return ImGuiKey_Pause;
		case VK_NUMPAD0:
			return ImGuiKey_Keypad0;
		case VK_NUMPAD1:
			return ImGuiKey_Keypad1;
		case VK_NUMPAD2:
			return ImGuiKey_Keypad2;
		case VK_NUMPAD3:
			return ImGuiKey_Keypad3;
		case VK_NUMPAD4:
			return ImGuiKey_Keypad4;
		case VK_NUMPAD5:
			return ImGuiKey_Keypad5;
		case VK_NUMPAD6:
			return ImGuiKey_Keypad6;
		case VK_NUMPAD7:
			return ImGuiKey_Keypad7;
		case VK_NUMPAD8:
			return ImGuiKey_Keypad8;
		case VK_NUMPAD9:
			return ImGuiKey_Keypad9;
		case VK_DECIMAL:
			return ImGuiKey_KeypadDecimal;
		case VK_DIVIDE:
			return ImGuiKey_KeypadDivide;
		case VK_MULTIPLY:
			return ImGuiKey_KeypadMultiply;
		case VK_SUBTRACT:
			return ImGuiKey_KeypadSubtract;
		case VK_ADD:
			return ImGuiKey_KeypadAdd;
		case IM_VK_KEYPAD_ENTER:
			return ImGuiKey_KeypadEnter;
		case VK_LSHIFT:
			return ImGuiKey_LeftShift;
		case VK_LCONTROL:
			return ImGuiKey_LeftCtrl;
		case VK_LMENU:
			return ImGuiKey_LeftAlt;
		case VK_LWIN:
			return ImGuiKey_LeftSuper;
		case VK_RSHIFT:
			return ImGuiKey_RightShift;
		case VK_RCONTROL:
			return ImGuiKey_RightCtrl;
		case VK_RMENU:
			return ImGuiKey_RightAlt;
		case VK_RWIN:
			return ImGuiKey_RightSuper;
		case VK_APPS:
			return ImGuiKey_Menu;
		case '0':
			return ImGuiKey_0;
		case '1':
			return ImGuiKey_1;
		case '2':
			return ImGuiKey_2;
		case '3':
			return ImGuiKey_3;
		case '4':
			return ImGuiKey_4;
		case '5':
			return ImGuiKey_5;
		case '6':
			return ImGuiKey_6;
		case '7':
			return ImGuiKey_7;
		case '8':
			return ImGuiKey_8;
		case '9':
			return ImGuiKey_9;
		case 'A':
			return ImGuiKey_A;
		case 'B':
			return ImGuiKey_B;
		case 'C':
			return ImGuiKey_C;
		case 'D':
			return ImGuiKey_D;
		case 'E':
			return ImGuiKey_E;
		case 'F':
			return ImGuiKey_F;
		case 'G':
			return ImGuiKey_G;
		case 'H':
			return ImGuiKey_H;
		case 'I':
			return ImGuiKey_I;
		case 'J':
			return ImGuiKey_J;
		case 'K':
			return ImGuiKey_K;
		case 'L':
			return ImGuiKey_L;
		case 'M':
			return ImGuiKey_M;
		case 'N':
			return ImGuiKey_N;
		case 'O':
			return ImGuiKey_O;
		case 'P':
			return ImGuiKey_P;
		case 'Q':
			return ImGuiKey_Q;
		case 'R':
			return ImGuiKey_R;
		case 'S':
			return ImGuiKey_S;
		case 'T':
			return ImGuiKey_T;
		case 'U':
			return ImGuiKey_U;
		case 'V':
			return ImGuiKey_V;
		case 'W':
			return ImGuiKey_W;
		case 'X':
			return ImGuiKey_X;
		case 'Y':
			return ImGuiKey_Y;
		case 'Z':
			return ImGuiKey_Z;
		case VK_F1:
			return ImGuiKey_F1;
		case VK_F2:
			return ImGuiKey_F2;
		case VK_F3:
			return ImGuiKey_F3;
		case VK_F4:
			return ImGuiKey_F4;
		case VK_F5:
			return ImGuiKey_F5;
		case VK_F6:
			return ImGuiKey_F6;
		case VK_F7:
			return ImGuiKey_F7;
		case VK_F8:
			return ImGuiKey_F8;
		case VK_F9:
			return ImGuiKey_F9;
		case VK_F10:
			return ImGuiKey_F10;
		case VK_F11:
			return ImGuiKey_F11;
		case VK_F12:
			return ImGuiKey_F12;
		default:
			return ImGuiKey_None;
		}
	}

	// Translates Virtual Key Codes into Skyrim compatible codes for Skyrim specific Input.
	// This is needed to recieve the correct key codes for opening the menu.
	static inline uint32_t VirtualKeyToSkyrim(WPARAM wParam)
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
		case IM_VK_KEYPAD_ENTER:
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

	// Specifically used for mapping Key Codes for ImGuiIcon Library.
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

	// Map of Skyrim Key Codes to Key Names.
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