#include "include/M/Menu.h"
#include "include/C/Console.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/K/Keycode.h"
#include "include/U/UserSettings.h"
#include <dinput.h>

namespace Modex
{

	void Menu::Open()
	{
		isEnabled = true;
		prevFreezeState = RE::Main::GetSingleton()->freezeTime;

		if (Settings::GetSingleton()->GetConfig().pauseGame) {
			RE::Main::GetSingleton()->freezeTime = true;
		}

		ImGui::SetWindowFocus(NULL);  // Reset InputBox Focus
	}

	void Menu::Close()
	{
		OnFocusKill();
		RE::Main::GetSingleton()->freezeTime = prevFreezeState;

		isEnabled = false;
	}

	void Menu::Toggle()
	{
		if (isEnabled) {
			Close();
		} else {
			Open();
		}
	}

	void Menu::Draw()
	{
		// TODO: Maybe hook this into an update call?
		Console::ProcessMainThreadTasks();

		if (!isEnabled) {
			return;
		}

		if (pendingFontChange) {
			RebuildFontAtlas();

			return;
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();

		if (isEnabled) {
			ImGui::GetIO().MouseDrawCursor = true;
		} else {
			ImGui::GetIO().MouseDrawCursor = false;
		}

		Frame::Draw(showSettingWindow);

		//ImGui::ShowDemoWindow();

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void Menu::RefreshFont()
	{
		pendingFontChange = true;
	}

	void Menu::RebuildFontAtlas()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		FontManager::GetSingleton()->SetStartupFont();

		io.Fonts->Build();
		ImGui_ImplDX11_InvalidateDeviceObjects();

		pendingFontChange = false;
	}

	void Menu::Init(IDXGISwapChain* a_swapchain, ID3D11Device* a_device, ID3D11DeviceContext* a_context)
	{
		IMGUI_CHECKVERSION();

		DXGI_SWAP_CHAIN_DESC desc;
		a_swapchain->GetDesc(&desc);

		ImGui::CreateContext();

		RECT rect{};
		ImVec2 screenScaleRatio;
		if (GetClientRect(desc.OutputWindow, &rect) == TRUE) {
			screenScaleRatio = ImVec2{ static_cast<float>(desc.BufferDesc.Width) / static_cast<float>(rect.right), static_cast<float>(desc.BufferDesc.Height) / static_cast<float>(rect.bottom) };
		} else {
			screenScaleRatio = ImVec2{ 1.0f, 1.0f };
		}

		Settings::GetSingleton()->GetConfig().screenScaleRatio = screenScaleRatio;

		ImGui_ImplWin32_Init(desc.OutputWindow);
		ImGui_ImplDX11_Init(a_device, a_context);

		this->device = a_device;
		this->context = a_context;
	}

	void Menu::RefreshStyle()
	{
		auto& style = Settings::GetSingleton()->GetStyle();

		SyncUserStyleToImGui(style);

		Frame::RefreshStyle();
	}

	void Menu::SyncUserStyleToImGui(Settings::Style user)
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		// ImGui::GetIO().FontGlobalScale = user.globalFontSize;

		colors[ImGuiCol_Text] = user.text;
		colors[ImGuiCol_TextDisabled] = user.textDisabled;
		colors[ImGuiCol_WindowBg] = user.windowBg;
		colors[ImGuiCol_Border] = user.border;
		colors[ImGuiCol_FrameBg] = user.widgetBg;
		colors[ImGuiCol_FrameBgHovered] = user.widgetHovered;
		colors[ImGuiCol_FrameBgActive] = user.widgetHovered;
		colors[ImGuiCol_ScrollbarBg] = user.scrollbarBg;
		colors[ImGuiCol_ScrollbarGrab] = user.scrollbarGrab;
		colors[ImGuiCol_ScrollbarGrabHovered] = user.scrollbarGrab;
		colors[ImGuiCol_ScrollbarGrabActive] = user.scrollbarGrab;
		colors[ImGuiCol_CheckMark] = user.checkMark;
		colors[ImGuiCol_SliderGrab] = user.sliderGrab;
		colors[ImGuiCol_SliderGrabActive] = user.sliderGrab;
		colors[ImGuiCol_Button] = user.button;
		colors[ImGuiCol_ButtonHovered] = user.buttonHovered;
		colors[ImGuiCol_ButtonActive] = user.buttonActive;
		colors[ImGuiCol_Header] = user.header;
		colors[ImGuiCol_HeaderHovered] = user.headerHovered;
		colors[ImGuiCol_HeaderActive] = user.headerActive;
		colors[ImGuiCol_Separator] = user.separator;
		colors[ImGuiCol_SeparatorHovered] = user.separatorHovered;
		colors[ImGuiCol_SeparatorActive] = user.separatorActive;
		colors[ImGuiCol_ResizeGrip] = user.resizeGrip;
		colors[ImGuiCol_ResizeGripHovered] = user.resizeGripHovered;
		colors[ImGuiCol_ResizeGripActive] = user.resizeGripActive;
		colors[ImGuiCol_TableHeaderBg] = user.tableHeaderBg;
		colors[ImGuiCol_TableBorderStrong] = user.tableBorderStrong;
		colors[ImGuiCol_TableBorderLight] = user.tableBorderLight;
		colors[ImGuiCol_TableRowBg] = user.tableRowBg;
		colors[ImGuiCol_TextSelectedBg] = user.textSelectedBg;

		style.WindowPadding = user.windowPadding;
		style.FramePadding = user.widgetPadding;
		style.CellPadding = user.cellPadding;
		style.ItemSpacing = user.itemSpacing;
		style.ItemInnerSpacing = user.itemInnerSpacing;

		style.WindowRounding = user.windowRounding;
		style.WindowBorderSize = user.windowBorderSize;
		style.FrameBorderSize = user.widgetBorderSize;
		style.FrameRounding = user.widgetRounding;
		style.IndentSpacing = user.indentSpacing;
		style.ColumnsMinSpacing = user.columnsMinSpacing;
		style.ScrollbarRounding = user.scrollbarRounding;
		style.ScrollbarSize = user.scrollbarSize;
		style.GrabMinSize = user.grabMinSize;
		style.GrabRounding = user.scrollbarRounding;
	}

	class CharEvent : public RE::InputEvent
	{
	public:
		uint32_t keyCode;  // 18 (ascii code)
	};

	enum : std::uint32_t
	{
		kInvalid = static_cast<std::uint32_t>(-1),
		kKeyboardOffset = 0,
		kMouseOffset = 256,
		kGamepadOffset = 266
	};

	void Menu::OnFocusKill()
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

	void Menu::ProcessInputEvent(RE::InputEvent** a_event)
	{
		if (!a_event)
			return;

		auto& io = ImGui::GetIO();

		// Loop through inputEvents passed by the game.
		for (auto inputEvent = *a_event; inputEvent; inputEvent = inputEvent->next) {
			if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kChar) {
				if (isEnabled) {
					io.AddInputCharacter(static_cast<CharEvent*>(inputEvent)->keyCode);
				}
			} else if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kButton) {
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

				uint32_t showMenuKey = Settings::GetSingleton()->GetConfig().showMenuKey;
				uint32_t showMenuModifier = Settings::GetSingleton()->GetConfig().showMenuModifier;
				const ImGuiKey imGuiKey = ImGui::VirtualKeyToImGuiKey(key);

				// Detect and pass modifier keys to ImGui IO.
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

				switch (buttonEvent->device.get()) {
				case RE::INPUT_DEVICE::kMouse:
					if (isEnabled) {
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
				case RE::INPUT_DEVICE::kKeyboard:
					if (isEnabled) {
						io.AddKeyEvent(imGuiKey, buttonEvent->IsPressed());

						if (scanCode == uint32_t(41) && buttonEvent->IsDown()) {  // Console
							Close();
						}

						if (scanCode == uint32_t(1) && buttonEvent->IsDown()) {  // Escape
							Close();
						}
					}

					if (scanCode == showMenuKey && buttonEvent->IsDown()) {  // Open / Close Menu
						if (showMenuModifier == 0) {
							Toggle();
						} else {
							logger::info("showMenuModifier: {}", showMenuModifier);
							if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LSHIFT) && shiftDown) {
								Toggle();
							} else if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LCONTROL) && ctrlDown) {
								Toggle();
							} else if (showMenuModifier == (uint32_t)ImGui::VirtualKeyToSkyrim(VK_LMENU) && altDown) {
								Toggle();
							}
						}
					}

					break;
				case RE::INPUT_DEVICE::kGamepad:  // Not implemented
					break;
				default:
					continue;
				}
			}
		}
		return;
	}
}