#include "Menu.h"
#include "Console.h"
#include "Utils/Keycode.h"
#include "Windows/Frame.h"
#include "Windows/UserSettings/UserSettings.h"
#include <dinput.h>

namespace Modex
{
	void Menu::Draw()
	{
		// TODO: Maybe hook this into an update call?
		Console::ProcessMainThreadTasks();

		if (!IsEnabled()) {
			return;  // TODO: Should this be called one level up?
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		if (IsEnabled()) {
			ImGui::GetIO().MouseDrawCursor = true;
		} else {
			ImGui::GetIO().MouseDrawCursor = false;
		}

		Settings::Style& style = Settings::GetSingleton()->GetStyle();

		ImGui::PushFont(style.font.normal);
		Frame::Draw(is_settings_popped);
		ImGui::PopFont();

		//ImGui::ShowDemoWindow();

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void Menu::Init(IDXGISwapChain* a_swapchain, ID3D11Device* a_device, ID3D11DeviceContext* a_context)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();

		DXGI_SWAP_CHAIN_DESC desc;
		a_swapchain->GetDesc(&desc);

		ImGui_ImplWin32_Init(desc.OutputWindow);
		ImGui_ImplDX11_Init(a_device, a_context);

		//initialized.store(true);

		this->device = a_device;
		this->context = a_context;
	}

	Menu::~Menu()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void Menu::RefreshStyle()
	{
		auto& style = Settings::GetSingleton()->GetStyle();
		//auto& config = Settings::GetSingleton()->GetConfig();

		//Settings::SetThemeFromIni(config.theme);
		SyncUserStyleToImGui(style);

		Frame::RefreshStyle();
	}

	void Menu::SyncUserStyleToImGui(Settings::Style user)
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		ImGui::GetIO().FontGlobalScale = user.globalFontSize;

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

	void Menu::ProcessInputEvent(RE::InputEvent** a_event)
	{
		if (!a_event)
			return;

		auto& io = ImGui::GetIO();

		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType == RE::INPUT_EVENT_TYPE::kChar) {
				if (!Menu::IsEnabled()) {
					continue;
				}

				io.AddInputCharacter(static_cast<CharEvent*>(event)->keyCode);
			} else if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
				const auto button = static_cast<RE::ButtonEvent*>(event);
				if (!button || (button->IsPressed() && !button->IsDown()))
					continue;

				auto scan_code = button->GetIDCode();

				using DeviceType = RE::INPUT_DEVICE;
				auto input = scan_code;
				switch (button->device.get()) {
				case DeviceType::kMouse:
					input += kMouseOffset;
					break;
				case DeviceType::kKeyboard:
					input += kKeyboardOffset;
					break;
				case DeviceType::kGamepad:
					continue;
					break;
				default:
					continue;
				}

				uint32_t key = MapVirtualKeyEx(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
				switch (scan_code) {
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

				switch (button->device.get()) {
				case RE::INPUT_DEVICE::kMouse:
					if (!Menu::IsEnabled()) {
						continue;
					}

					if (scan_code > 7)  // middle scroll
						io.AddMouseWheelEvent(0, button->Value() * (scan_code == 8 ? 1 : -1));
					else {
						if (scan_code > 5)
							scan_code = 5;
						io.AddMouseButtonEvent(scan_code, button->IsPressed());
					}
					break;
				case RE::INPUT_DEVICE::kKeyboard:
					if (Menu::IsEnabled()) {
						io.AddKeyEvent(ImGui::VirtualKeyToImGuiKey(key), button->IsPressed());
					}

					if (button->GetIDCode() == showMenuModifier) {
						if (button->IsPressed()) {
							Menu::is_modifier_pressed = true;
						} else {
							Menu::is_modifier_pressed = false;
						}
					}

					if (button->GetIDCode() == showMenuKey) {
						if (button->IsDown() && Menu::initialized.load()) {
							if (showMenuModifier == 0) {
								Menu::Toggle();
							} else if (Menu::is_modifier_pressed == true) {
								Menu::Toggle();
							}
						}
					} else if (button->GetIDCode() == uint32_t(41)) {  // console escape.
						if (button->IsDown() && Menu::IsEnabled()) {
							Menu::SetEnabled(false);
						}
					}
					break;
				case RE::INPUT_DEVICE::kGamepad:
					// not implemented
					break;
				default:
					continue;
				}
			}
		}
		return;
	}
}