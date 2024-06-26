#include "Frame.h"
#include "Graphic.h"
#include "Settings.h"
#include "Windows/AddItem/AddItem.h"
#include "Windows/Cheat/Cheat.h"
#include "Windows/Home/Home.h"
#include "Windows/NPC/NPC.h"
#include "Windows/Object/Object.h"
#include "Windows/Teleport/Teleport.h"
#include "Windows/UserSettings/UserSettings.h"

namespace ModExplorerMenu
{
	Frame::ActiveWindow Frame::_activeWindow = Frame::ActiveWindow::Home;

	struct Properties
	{
		ImVec2 screenSize;
		float sidebar_w;
		float sidebar_h;
		float panel_w;
		float panel_h;

		Properties()
		{
			screenSize = ImGui::GetMainViewport()->Size;
			sidebar_w = screenSize.x * 0.10f;
			sidebar_h = screenSize.y * 0.65f;
			panel_w = screenSize.x * 0.50f;
			panel_h = screenSize.y * 0.65f;
		}
	};

	void Frame::Draw(bool is_settings_popped)
	{
		auto& style = Settings::GetSingleton()->GetStyle();
		auto& config = Settings::GetSingleton()->GetConfig();

		if (is_settings_popped) {
			SettingsWindow::DrawPopped();
		}

		Properties window;
		const float center_x = window.screenSize.x * 0.5f;
		const float center_y = (window.screenSize.y * 0.5f) - (window.panel_h * 0.5f);
		const float panel_x = center_x - (window.panel_w * 0.5f) + (window.sidebar_w * 0.5f);
		const float sidebar_x = panel_x - (window.sidebar_w);

		// Draw Sidebar Frame
		static constexpr ImGuiWindowFlags sidebar_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
		auto noFocus = is_settings_popped ? ImGuiWindowFlags_NoBringToFrontOnFocus : 0;
		ImGui::SetNextWindowSize(ImVec2(window.sidebar_w, window.sidebar_h));
		ImGui::SetNextWindowPos(ImVec2(sidebar_x, center_y));

		ImGui::PushFont(style.font.medium);
		if (ImGui::Begin("##AddItemMenuSideBar", NULL, sidebar_flag + noFocus)) {
			if (ImGui::Button("Home", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Home;
			}

			if (ImGui::Button("Add Item", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::AddItem;
			}

			if (ImGui::Button("Object", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Object;
			}

			if (ImGui::Button("NPC", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::NPC;
			}

			if (ImGui::Button("Lookup", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Lookup;
			}

			if (ImGui::Button("Teleport", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Teleport;
			}

			if (ImGui::Button("Cheat", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Cheat;
			}

			if (ImGui::Button("Settings", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f))) {
				_activeWindow = ActiveWindow::Settings;
			}

			ImGui::End();
		}
		ImGui::PopFont();

		// Draw Panel Frame conditionally
		static constexpr ImGuiWindowFlags panel_flag = sidebar_flag;
		ImGui::SetNextWindowSize(ImVec2(window.panel_w, window.panel_h));
		ImGui::SetNextWindowPos(ImVec2(panel_x, center_y));

		ImGui::PushFont(style.font.nano);
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
			case ActiveWindow::Lookup:
				break;
			case ActiveWindow::Teleport:
				TeleportWindow::Draw(style, config);
				break;
			case ActiveWindow::Cheat:
				CheatWindow::Draw(style, config);
				break;
			case ActiveWindow::Settings:
				SettingsWindow::Draw();
				break;
			}

			ImGui::End();
		}
		ImGui::PopFont();

		GraphicManager::DrawImage(style.splashImage, ImVec2(window.screenSize.x * 0.5f, window.screenSize.y * 0.5f));
	}

	void Frame::Install()
	{
		// Initalize elements
		GraphicManager::Init();
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