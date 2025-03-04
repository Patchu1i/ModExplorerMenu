#include "include/I/InputManager.h"
#include "include/K/Keycode.h"
#include <dinput.h>

namespace Modex
{
	void InputManager::OnFocusKill()
	{
		// Issue #48 - Fix focus kill on menu close for Simple IME compatability.
		if (ImGui::GetCurrentContext() == nullptr) {
			return;
		}

		auto& io = ImGui::GetIO();
		io.ClearInputKeys();
		io.ClearEventsQueue();
		io.AddFocusEvent(false);

		shiftDown = false;
		ctrlDown = false;
		altDown = false;
		modifierDown = false;
	}

	void InputManager::UpdateSettings()
	{
		auto& config = Settings::GetSingleton()->GetConfig();
		showMenuKey = config.showMenuKey;
		showMenuModifier = config.showMenuModifier;
	}

	void InputManager::Init()
	{
		UpdateSettings();

		captureInput = false;
	}

	// TODO: Is this the best way (?)
	bool InputManager::ShouldProcessEvent(RE::InputEvent** a_event)
	{
		(void)a_event;

		// Don't need this fix if we have a modifier, since it wouldn't fire anyway.
		if (this->showMenuModifier != 0) {
			return true;
		}

		// If the hotkey assigned to Modex doesn't overlap text-input behavioral keys, then we can process the event.
		if (this->showMenuKey != 0x0E &&  // Backspace
			this->showMenuKey != 0x0F &&  // Tab
			this->showMenuKey != 0x3A &&  // Caps-Lock
			this->showMenuKey != 0x45 &&  // Num-Lock
			this->showMenuKey != 0x46 &&  // Scroll-Lock
			this->showMenuKey != 0xB7 &&  // Prnt-Scrn
			this->showMenuKey != 0xC5 &&  // Pause
			this->showMenuKey != 0xC7 &&  // Home
			this->showMenuKey != 0xC8 &&  // Up
			this->showMenuKey != 0xC9 &&  // Page-Up
			this->showMenuKey != 0xCB &&  // Left
			this->showMenuKey != 0xCD &&  // Right
			this->showMenuKey != 0xCF &&  // End
			this->showMenuKey != 0xD0 &&  // Down
			this->showMenuKey != 0xD1 &&  // Page-Down
			this->showMenuKey != 0xD2 &&  // Insert
			this->showMenuKey != 0xD3) {  // Delete
			return true;
		}

		const auto UIManager = RE::UI::GetSingleton();

		if (UIManager->IsMenuOpen("Console") ||         // Text Input
			UIManager->IsMenuOpen("Dialogue Menu") ||   // Dialogue
			UIManager->IsMenuOpen("Crafting Menu") ||   // Text Input
			UIManager->IsMenuOpen("Training Menu") ||   // Just Incase
			UIManager->IsMenuOpen("MagicMenu") ||       // Text Input
			UIManager->IsMenuOpen("Quantity Menu") ||   // Text Input
			UIManager->IsMenuOpen("RaceSex Menu") ||    // Text Input
			UIManager->IsMenuOpen("BarterMenu") ||      // Text Input
			UIManager->IsMenuOpen("InventoryMenu") ||   // Text Input
			UIManager->IsMenuOpen("ContainerMenu") ||   // Text Input
			UIManager->IsMenuOpen("MessageBoxMenu")) {  // Text Input
			return false;
		}

		return true;
	}

	void InputManager::ProcessInputEvent(RE::InputEvent** a_event)
	{
		// Since inputs can be listened to outside of the menu. Issue #48
		if (ImGui::GetCurrentContext() == nullptr) {
			return;
		}

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

					// 0x0F = Skyrim "Tab" key.
					// 0x38 = Skyrim "L-Alt" key.
					if (lastKeyPress == 0x0F) {
						if (scanCode == 0x38 && buttonEvent->IsPressed()) {
							lastKeyPress = 0;

							io.ClearInputKeys();
						}
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
								if (ImGui::IsPopupOpen("IgnoreCloseEvent", ImGuiPopupFlags_AnyPopup)) {
									io.AddKeyEvent(imGuiKey, buttonEvent->IsPressed());
									break;
								}

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

							lastKeyPress = scanCode;

							break;
						}
					}
				}
			}
		}
	}
}