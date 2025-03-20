#include "include/M/Menu.h"
#include "include/C/Console.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/InputManager.h"
#include "include/P/Papyrus.h"
#include "include/U/UIManager.h"
#include "include/U/UserSettings.h"

namespace Modex
{
	bool Menu::AllowMenuOpen()
	{
		const auto& config = Settings::GetSingleton()->GetConfig();

		if (!config.disableInMenu)
			return true;

		if (config.showMenuModifier != 0)
			return true;

		if (Menu::GetSingleton()->isEnabled)
			return true;

		// If the hotkey assigned to Modex doesn't overlap text-input behavioral keys, then we can process the event.
		if (config.showMenuKey != 0x0E &&  // Backspace
			config.showMenuKey != 0x0F &&  // Tab
			config.showMenuKey != 0x3A &&  // Caps-Lock
			config.showMenuKey != 0x45 &&  // Num-Lock
			config.showMenuKey != 0x46 &&  // Scroll-Lock
			config.showMenuKey != 0xB7 &&  // Prnt-Scrn
			config.showMenuKey != 0xC5 &&  // Pause
			config.showMenuKey != 0xC7 &&  // Home
			config.showMenuKey != 0xC8 &&  // Up
			config.showMenuKey != 0xC9 &&  // Page-Up
			config.showMenuKey != 0xCB &&  // Left
			config.showMenuKey != 0xCD &&  // Right
			config.showMenuKey != 0xCF &&  // End
			config.showMenuKey != 0xD0 &&  // Down
			config.showMenuKey != 0xD1 &&  // Page-Down
			config.showMenuKey != 0xD2 &&  // Insert
			config.showMenuKey != 0xD3) {  // Delete
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

	void Menu::Open()
	{
		if (!AllowMenuOpen()) {
			return;
		}

		if (RE::Main* Game = RE::Main::GetSingleton()) {
			prevFreezeState = Game->freezeTime;

			if (Settings::GetSingleton()->GetConfig().pauseGame) {
				Game->freezeTime = true;
			}
		}

		if (ImGui::GetCurrentContext() != nullptr) {
			auto& io = ImGui::GetIO();

			io.MouseDrawCursor = true;
			io.ClearInputKeys();

			// TODO: Revisit when assigning default focus on open/close.
			ImGui::SetWindowFocus(NULL);
		}

		isEnabled = true;
	}

	void Menu::Close()
	{
		if (RE::Main* Game = RE::Main::GetSingleton()) {
			Game->freezeTime = prevFreezeState;
		}

		if (ImGui::GetCurrentContext() != nullptr) {
			auto& io = ImGui::GetIO();

			io.MouseDrawCursor = false;
			io.ClearInputKeys();
		}

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
		Console::ProcessMainThreadTasks();
		InputManager::ProcessInput();

		if (!isEnabled && !PapyrusWindow::GetSingleton()->IsOverlayEnabled()) {
			return;
		}

		if (pendingFontChange) {
			RebuildFontAtlas();
			return;
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();

		if (!isEnabled) {
			if (const auto& ProfilerUI = PapyrusWindow::GetSingleton(); ProfilerUI) {
				if (ProfilerUI->IsOverlayEnabled()) {
					ProfilerUI->DrawOverlay();
				}
			}
		} else {
			Frame::GetSingleton()->Draw(showSettingWindow);
			UIManager::GetSingleton()->Draw();
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void Menu::Update()
	{
		if (const auto& ProfilerUI = PapyrusWindow::GetSingleton(); ProfilerUI) {
			ProfilerUI->Update();
		}
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

		io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
		io.Fonts->FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;

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

		this->device = a_device;  // (?)
		this->context = a_context;
	}

	void Menu::RefreshStyle()
	{
		auto& style = Settings::GetSingleton()->GetStyle();

		SyncUserStyleToImGui(style);

		Frame::GetSingleton()->RefreshStyle();
	}

	void Menu::SyncUserStyleToImGui(Settings::Style user)
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

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
}