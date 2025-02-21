#include "include/M/Menu.h"
#include "include/C/Console.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/InputManager.h"
#include "include/U/UserSettings.h"

namespace Modex
{

	void Menu::Open()
	{
		isEnabled = true;
		prevFreezeState = RE::Main::GetSingleton()->freezeTime;

		if (Settings::GetSingleton()->GetConfig().pauseGame) {
			RE::Main::GetSingleton()->freezeTime = true;
		}

		InputManager::GetSingleton()->captureInput = true;

		// ImGui::SetWindowFocus(NULL);  // Reset InputBox Focus
	}

	void Menu::Close()
	{
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
			ImGui::GetIO().MouseDrawCursor = false;
			InputManager::GetSingleton()->captureInput = false;
			return;
		}

		if (pendingFontChange) {
			RebuildFontAtlas();
			return;
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();
		ImGui::GetIO().MouseDrawCursor = true;

		Frame::GetSingleton()->Draw(showSettingWindow);

		// ImGui::ShowDemoWindow(); // This could be added to the settings?

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