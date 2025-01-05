#include "Frame.h"
#include "Graphic.h"
#include "Menu.h"
#include "Utils/Util.h"
#include "Windows/AddItem/AddItem.h"
#include "Windows/Home/Home.h"
#include "Windows/NPC/NPC.h"
#include "Windows/Object/Object.h"
#include "Windows/Teleport/Teleport.h"
#include "Windows/UserSettings/UserSettings.h"

namespace Modex
{
	struct WindowProperties
	{
		float sidebar_w;
		float sidebar_h;
		float panel_w;
		float panel_h;
	};

	void Frame::Draw(bool is_settings_popped)
	{
		auto& style = Settings::GetSingleton()->GetStyle();
		auto& config = Settings::GetSingleton()->GetConfig();

		if (is_settings_popped) {
			SettingsWindow::DrawPopped();
		}

		ImGuiIO& io = ImGui::GetIO();

		// Set the window size based on the display size and the user's screen scale ratio.
		auto displaySize = io.DisplaySize;
		displaySize.x *= config.screenScaleRatio.x;
		displaySize.y *= config.screenScaleRatio.y;

		WindowProperties window;

		// Apply the user's UI scale config
		window.panel_w = (displaySize.x * 0.60f) * (config.uiScale / 100.0f);
		window.panel_h = (displaySize.y * 0.75f) * (config.uiScale / 100.0f);
		window.sidebar_w = (displaySize.x * 0.10f) * (config.uiScale / 100.0f);
		window.sidebar_h = (displaySize.y * 0.75f) * (config.uiScale / 100.0f);

		// Calculate window positions post scaling.
		float center_x = (displaySize.x * 0.5f);
		float center_y = (displaySize.y * 0.5f) - (window.panel_h * 0.5f);
		float panel_x = center_x - (window.panel_w * 0.5f) + (window.sidebar_w * 0.5f) + (style.sidebarSpacing / 2);
		float sidebar_x = panel_x - (window.sidebar_w) - (style.sidebarSpacing);

		// Draw a transparent black background.
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(displaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.35f));

		ImGui::Begin("##Background", NULL, BACKGROUND_FLAGS);
		ImGui::End();

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);

		// Draw Sidebar Frame
		static constexpr ImGuiWindowFlags sidebar_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
		auto noFocus = is_settings_popped ? ImGuiWindowFlags_NoBringToFrontOnFocus : 0;
		ImGui::SetNextWindowSize(ImVec2(window.sidebar_w, window.sidebar_h));
		ImGui::SetNextWindowPos(ImVec2(sidebar_x, center_y));

		ImGui::PushStyleColor(ImGuiCol_Header, style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style.buttonHovered);
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushFont(style.buttonFont.normal);
		if (ImGui::Begin("##AddItemMenuSideBar", NULL, sidebar_flag + noFocus)) {
			auto iWidth = ImGui::GetContentRegionAvail().x;
			auto iHeight = ImGui::GetWindowSize().y / 12;
			ImGui::Image(GraphicManager::image_library["logo"].texture, ImVec2(iWidth, iHeight));

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			if (config.showHomeMenu) {
				if (ImGui::Selectable(_T("Home"), &b_Home, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
					_activeWindow = ActiveWindow::Home;
					ResetSelectable();
				}
			}

			if (config.showAddItemMenu) {
				if (ImGui::Selectable(_T("Add Item"), &b_AddItem, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
					_activeWindow = ActiveWindow::AddItem;
					AddItemWindow::Refresh();
					ResetSelectable();
				}
			}

			if (config.showObjectMenu) {
				if (ImGui::Selectable(_T("Object"), &b_Object, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
					_activeWindow = ActiveWindow::Object;
					ObjectWindow::Refresh();
					ResetSelectable();
				}
			}

			if (config.showNPCMenu) {
				if (ImGui::Selectable(_T("NPC"), &b_NPC, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
					_activeWindow = ActiveWindow::NPC;
					NPCWindow::Refresh();
					ResetSelectable();
				}
			}

			if (config.showTeleportMenu) {
				if (ImGui::Selectable(_T("Teleport"), &b_Teleport, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
					_activeWindow = ActiveWindow::Teleport;
					TeleportWindow::Refresh();
					ResetSelectable();
				}
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			if (ImGui::Selectable(_T("Settings"), &b_Settings, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Settings;
				ResetSelectable();
			}

			if (ImGui::Selectable(_T("Exit"), false, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				Menu::GetSingleton()->Toggle();
			}

			ImGui::End();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(1);

		// Draw Panel Frame
		static constexpr ImGuiWindowFlags panel_flag = sidebar_flag;
		ImGui::SetNextWindowSize(ImVec2(window.panel_w, window.panel_h));
		ImGui::SetNextWindowPos(ImVec2(panel_x, center_y));

		if (ImGui::Begin("##AddItemMenuPanel", NULL, sidebar_flag + noFocus + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoScrollWithMouse)) {
			switch (_activeWindow) {
			case ActiveWindow::Home:
				HomeWindow::Draw();
				break;
			case ActiveWindow::AddItem:
				AddItemWindow::Draw(style, config);
				break;
			case ActiveWindow::Object:
				ObjectWindow::Draw(style, config);
				break;
			case ActiveWindow::NPC:
				NPCWindow::Draw(style, config);
				break;
			case ActiveWindow::Teleport:
				TeleportWindow::Draw(style, config);
				break;
			case ActiveWindow::Settings:
				SettingsWindow::Draw();
				break;
			}

			ImGui::End();
		}

		GraphicManager::DrawImage(style.splashImage, ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f));
	}

	void Frame::Install()
	{
		Frame::_activeWindow = (ActiveWindow)Settings::GetSingleton()->GetConfig().defaultShow;
		ResetSelectable();

		// Initalize elements
		AddItemWindow::Init();
		HomeWindow::Init();
		NPCWindow::Init();
		TeleportWindow::Init();
		ObjectWindow::Init();

		RefreshStyle();

		// FIXME: This shouldn't be needed anymore
		Frame::_init.store(true);
	}

	void Frame::RefreshStyle()
	{
		// AddItemWindow::RefreshStyle();
		//HomeWindow::RefreshStyle();
		//SettingsWindow::RefreshStyle();

		// auto& style = Settings::GetSingleton()->GetStyle();
	}
}