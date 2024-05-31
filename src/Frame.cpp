#include "Frame.h"
#include "Settings.h"
#include "lib/Graphic.h"
#include "windows/AddItemWindow.h"
#include "windows/HomeWindow.h"
#include "windows/SettingsWindow.h"

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

void Frame::Draw()
{
	Properties window;

	auto& style = Settings::GetSingleton()->GetStyle();

	// Doing this here to prevent edge cases of Skyrim window resizing.
	const float center_x = window.screenSize.x * 0.5f;
	const float center_y = (window.screenSize.y * 0.5f) - (window.panel_h * 0.5f);
	const float panel_x = center_x - (window.panel_w * 0.5f) + (window.sidebar_w * 0.5f);
	const float sidebar_x = panel_x - (window.sidebar_w);

	// Draw Sidebar Frame
	static constexpr ImGuiWindowFlags sidebar_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
	ImGui::SetNextWindowSize(ImVec2(window.sidebar_w, window.sidebar_h));
	ImGui::SetNextWindowPos(ImVec2(sidebar_x, center_y));

	ImGui::PushFont(style.sidebarFont);
	if (ImGui::Begin("##AddItemMenuSideBar", NULL, sidebar_flag)) {
		if (ImGui::Button("Home", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::Home;
		}

		if (ImGui::Button("Add Item", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::AddItem;
		}

		if (ImGui::Button("NPC", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::NPC;
		}

		if (ImGui::Button("Lookup", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::Lookup;
		}

		if (ImGui::Button("Teleport", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::Teleport;
		}

		if (ImGui::Button("Settings", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::Settings;
		}

		ImGui::End();
	}
	ImGui::PopFont();

	// Draw Panel Frame conditionally
	static constexpr ImGuiWindowFlags panel_flag = sidebar_flag;
	ImGui::SetNextWindowSize(ImVec2(window.panel_w, window.panel_h));
	ImGui::SetNextWindowPos(ImVec2(panel_x, center_y));

	ImGui::PushFont(style.textFont);
	if (ImGui::Begin("##AddItemMenuPanel", NULL, sidebar_flag)) {
		switch (_activeWindow) {
		case ActiveWindow::Home:
			HomeWindow::Draw();
			break;
		case ActiveWindow::AddItem:
			AddItemWindow::Draw();
			break;
		case ActiveWindow::NPC:
			break;
		case ActiveWindow::Lookup:
			break;
		case ActiveWindow::Teleport:
			break;
		case ActiveWindow::Settings:
			SettingsWindow::Draw();
			break;
		}

		ImGui::Image(Frame::header_texture.texture, ImVec2(100, 100));

		ImGui::End();
	}
	ImGui::PopFont();

	GraphicManager::DrawImage(Frame::header_texture, ImVec2(window.screenSize.x * 0.5f, window.screenSize.y * 0.5f));
}

void Frame::Install()
{
	// Initalize elements
	GraphicManager::Init();
	AddItemWindow::Init();
	HomeWindow::Init();

	RefreshStyle();

	// Setup textures
	Frame::header_texture = GraphicManager::GetImage("bg-skyrim-540");

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