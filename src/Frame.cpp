#	include "Frame.h"
#	include "lib/Graphic.h"
#	include "windows/AddItemWindow.h"
#	include "windows/HomeWindow.h"

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
		sidebar_h = screenSize.y * 0.50f;
		panel_w = screenSize.x * 0.50f;
		panel_h = screenSize.y * 0.50f;
	}
};

void Frame::Draw()
{
	Properties window;

	// Doing this here to prevent edge cases of Skyrim window resizing.
	const float center_x = window.screenSize.x * 0.5f;
	const float center_y = (window.screenSize.y * 0.5f) - (window.panel_h * 0.5f);
	const float panel_x = center_x - (window.panel_w * 0.5f) + (window.sidebar_w * 0.5f);
	const float sidebar_x = panel_x - (window.sidebar_w);

	// Draw Sidebar Frame
	static constexpr ImGuiWindowFlags sidebar_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
	ImGui::SetNextWindowSize(ImVec2(window.sidebar_w, window.sidebar_h));
	ImGui::SetNextWindowPos(ImVec2(sidebar_x, center_y));

	if (ImGui::Begin("##AddItemMenuSideBar", NULL, sidebar_flag)) {
		if (ImGui::Button("Home", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::Home;
		}

		if (ImGui::Button("Add Item", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			_activeWindow = ActiveWindow::AddItem;
		}

		ImGui::End();
	}

	// Draw Panel Frame conditionally
	static constexpr ImGuiWindowFlags panel_flag = sidebar_flag;
	ImGui::SetNextWindowSize(ImVec2(window.panel_w, window.panel_h));
	ImGui::SetNextWindowPos(ImVec2(panel_x, center_y));

	if (ImGui::Begin("##AddItemMenuPanel", NULL, sidebar_flag)) {
		switch (_activeWindow) {
		case ActiveWindow::Home:
			HomeWindow::Draw();
			break;
		case ActiveWindow::AddItem:
			AddItemWindow::Draw();
			break;
		}

		ImGui::End();
	}

	// Draw header texture above frames.
	const float screen_size_x = window.panel_w + window.sidebar_w;
	const float screen_size_y = 50.0f;
	static constexpr ImGuiWindowFlags header_flag = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
	ImGui::SetNextWindowSize(ImVec2(screen_size_x, screen_size_y));
	ImGui::SetNextWindowPos(ImVec2(sidebar_x, center_y - screen_size_y));

	if (ImGui::Begin("##AddItemMenuHeader", nullptr, header_flag)) {
		ImGui::Image(header_texture, ImVec2(screen_size_x * 0.35f, screen_size_y * 0.80f));
		ImGui::End();
	}
}

void Frame::Install()
{
	// Initalize elements
	GraphicManager::Init();
	AddItemWindow::Init();
	HomeWindow::Init();

	// Setup textures
	Frame::header_texture = GraphicManager::GetImage("mod-explorer-menu-header").texture;

	// Let Input know we're done. TO-DO: Probably not needed.
	Frame::_init.store(true);
}