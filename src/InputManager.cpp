#include "include/I/InputManager.h"
#include "include/K/Keycode.h"
#include <dinput.h>

namespace Modex
{
	void InputManager::OnFocusKill()
	{
		auto& io = ImGui::GetIO();
		io.ClearInputKeys();
		io.ClearEventsQueue();
		io.ClearInputCharacters();  // TODO: Test if this is necessary(?)

		io.AddFocusEvent(false);

		shiftDown = false;
		ctrlDown = false;
		altDown = false;
		modifierDown = false;
	}

	void InputManager::Init()
	{
		auto& config = Settings::GetSingleton()->GetConfig();
		showMenuKey = config.showMenuKey;
		showMenuModifier = config.showMenuModifier;

		captureInput = false;
	}

	void InputManager::ProcessInputEvent(RE::InputEvent** a_event)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Loop through inputEvents incrementally.
		for (auto inputEvent = *a_event; inputEvent; inputEvent = inputEvent->next) {
			if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kChar) {
				{
					if (captureInput) {
						io.AddInputCharacter(static_cast<CharEvent*>(inputEvent)->keyCode);
					}
					break;
				}
			} else if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kButton) {
				{
					const RE::ButtonEvent* buttonEvent = static_cast<RE::ButtonEvent*>(inputEvent);
					const uint32_t scanCode = buttonEvent->GetIDCode();

					// Translate scan code to virtual key code.
					uint32_t key = MapVirtualKeyEx(scanCode, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
					switch (scanCode) {
					case DIK_LEFTARROW:
						key = VK_LEFT;
						break;
					case DIK_RIGHTARROW:
						key = VK_RIGHT;
						break;
					case DIK_UPARROW:
						key = VK_UP;
						break;
					case DIK_DOWNARROW:
						key = VK_DOWN;
						break;
					case DIK_DELETE:
						key = VK_DELETE;
						break;
					case DIK_END:
						key = VK_END;
						break;
					case DIK_HOME:
						key = VK_HOME;
						break;  // pos1
					case DIK_PRIOR:
						key = VK_PRIOR;
						break;  // page up
					case DIK_NEXT:
						key = VK_NEXT;
						break;  // page down
					case DIK_INSERT:
						key = VK_INSERT;
						break;
					case DIK_NUMPAD0:
						key = VK_NUMPAD0;
						break;
					case DIK_NUMPAD1:
						key = VK_NUMPAD1;
						break;
					case DIK_NUMPAD2:
						key = VK_NUMPAD2;
						break;
					case DIK_NUMPAD3:
						key = VK_NUMPAD3;
						break;
					case DIK_NUMPAD4:
						key = VK_NUMPAD4;
						break;
					case DIK_NUMPAD5:
						key = VK_NUMPAD5;
						break;
					case DIK_NUMPAD6:
						key = VK_NUMPAD6;
						break;
					case DIK_NUMPAD7:
						key = VK_NUMPAD7;
						break;
					case DIK_NUMPAD8:
						key = VK_NUMPAD8;
						break;
					case DIK_NUMPAD9:
						key = VK_NUMPAD9;
						break;
					case DIK_DECIMAL:
						key = VK_DECIMAL;
						break;
					case DIK_NUMPADENTER:
						key = IM_VK_KEYPAD_ENTER;
						break;
					case DIK_RMENU:
						key = VK_RMENU;
						break;  // right alt
					case DIK_RCONTROL:
						key = VK_RCONTROL;
						break;  // right control
					case DIK_LWIN:
						key = VK_LWIN;
						break;  // left win
					case DIK_RWIN:
						key = VK_RWIN;
						break;  // right win
					case DIK_APPS:
						key = VK_APPS;
						break;
					default:
						break;
					}

					// Modifier keys are not left/right side conscious.
					const ImGuiKey imGuiKey = ImGui::VirtualKeyToImGuiKey(key);
					if (imGuiKey == ImGuiKey_LeftShift || imGuiKey == ImGuiKey_RightShift) {
						shiftDown = buttonEvent->IsPressed();
					} else if (imGuiKey == ImGuiKey_LeftCtrl || imGuiKey == ImGuiKey_RightCtrl) {
						ctrlDown = buttonEvent->IsPressed();
					} else if (imGuiKey == ImGuiKey_LeftAlt || imGuiKey == ImGuiKey_RightAlt) {
						altDown = buttonEvent->IsPressed();
					}

					io.AddKeyEvent(ImGuiKey_ModShift, shiftDown);
					io.AddKeyEvent(ImGuiKey_ModCtrl, ctrlDown);
					io.AddKeyEvent(ImGuiKey_ModAlt, altDown);

					// We don't check if menu is shown since we clear the input
					// keys when the menu is closed or opened.

					switch (buttonEvent->device.get()) {
					case RE::INPUT_DEVICE::kMouse:
						{
							if (captureInput) {
								if (scanCode > 7)  // Middle Scroll
									io.AddMouseWheelEvent(0, buttonEvent->Value() * (scanCode == 8 ? 1 : -1));
								else {
									if (scanCode > 5) {
										io.AddMouseButtonEvent(5, buttonEvent->IsPressed());
									} else {
										io.AddMouseButtonEvent(scanCode, buttonEvent->IsPressed());
									}
								}
							}

							break;
						}
					case RE::INPUT_DEVICE::kKeyboard:
						{
							if (scanCode == 0x1 && buttonEvent->IsDown()) {
								Menu::GetSingleton()->Close();
							} else if (scanCode == showMenuKey && buttonEvent->IsDown()) {
								if (showMenuModifier == 0) {
									Menu::GetSingleton()->Toggle();
								} else {
									if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LSHIFT) && shiftDown) {
										Menu::GetSingleton()->Toggle();
									} else if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LCONTROL) && ctrlDown) {
										Menu::GetSingleton()->Toggle();
									} else if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LMENU) && altDown) {
										Menu::GetSingleton()->Toggle();
									}
								}
							} else {
								if (captureInput && scanCode != showMenuKey) {
									io.AddKeyEvent(imGuiKey, buttonEvent->IsPressed());
								}
							}

							break;
						}
					}
				}
			}
		}
	}
}