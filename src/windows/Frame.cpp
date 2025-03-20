#include "include/F/Frame.h"
#include "include/A/AddItem.h"
#include "include/B/Blacklist.h"
#include "include/H/Home.h"
#include "include/M/Menu.h"
#include "include/N/NPC.h"
#include "include/O/Object.h"
#include "include/P/Papyrus.h"
#include "include/S/Settings.h"
#include "include/T/Teleport.h"
#include "include/U/UserSettings.h"

namespace Modex
{

	void Frame::Draw(bool is_settings_popped)
	{
		auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();

		if (is_settings_popped) {
			SettingsWindow::DrawPopped();
		}

		ImGuiIO& io = ImGui::GetIO();

		// Set the window size based on the display size and the user's screen scale ratio.
		auto displaySize = io.DisplaySize;
		displaySize.x *= config.screenScaleRatio.x;
		displaySize.y *= config.screenScaleRatio.y;

		const float window_w = config.fullscreen ? displaySize.x : (displaySize.x * 0.80f) * (config.uiScale / 100.0f);
		const float window_h = config.fullscreen ? displaySize.y : (displaySize.y * 0.75f) * (config.uiScale / 100.0f);

		// const float min_sidebar_width = window_w * 0.060f;
		const float min_sidebar_width = 64.0f + (ImGui::GetStyle().WindowPadding.x * 2);
		// const float max_sidebar_width = window_w * 0.15f;
		const float max_sidebar_width = 210.0f;

		// Calculate window positions post scaling.
		const float center_x = (displaySize.x * 0.5f) - (window_w * 0.5f);
		const float center_y = (displaySize.y * 0.5f) - (window_h * 0.5f);

		// Draw a transparent black background.
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(displaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.35f));
		ImGui::Begin("##Background", nullptr, BACKGROUND_FLAGS);
		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);
		// End black background

		// If we're fullscreen, override manual positioning and sizing of the menu.
		ImGui::SetNextWindowSize(ImVec2(window_w, window_h));

		if ((sidebar_w == 0.0f || sidebar_h == 0.0f)) {
			sidebar_w = min_sidebar_width;
			sidebar_h = -1;
		}

		ImGui::SetNextWindowPos(ImVec2(center_x, center_y));
		if (ImGui::Begin("##ModexMenu", nullptr, WINDOW_FLAGS)) {
			ImGui::SetCursorPos(ImVec2(0, 0));

			ImGui::SetNextItemAllowOverlap();
			if (ImGui::BeginChild("##SideBar", ImVec2(sidebar_w, ImGui::GetContentRegionAvail().y + ImGui::GetStyle().WindowPadding.y), ImGuiChildFlags_Borders, SIDEBAR_FLAGS)) {
				const float button_width = ImGui::GetContentRegionAvail().x;
				constexpr float button_height = 40.0f;

				{
					// This is ridiculous, but is required due to Skyrim Upscaler Plugin. Typically, I could just
					// create a clip rect, or just hide the image behind the child window outside the sidebar. However,
					// when using Skyrim Upscaler Plugin. The texture clips over the bounds for some reason...
					ImTextureID texture = reinterpret_cast<ImTextureID>(GraphicManager::image_library["new_logo"].texture);

					// Using a fixed image width and height, since the sidebar is also a fixed width and height.
					constexpr float image_height = 54.0f;
					const float image_width = max_sidebar_width - ImGui::GetStyle().WindowPadding.x;
					const ImVec2 backup_pos = ImGui::GetCursorPos();

					// Calculate the UV for the image based on the sidebar width.
					float uv_x = 1.0f - std::min(0.35f, 1.0f - (sidebar_w / max_sidebar_width));
					uv_x *= (sidebar_w / image_width);

					// Minor tweak because of bad UV calculation.
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2.0f);
					// ImGui::Image(texture, ImVec2(image_width * (sidebar_w / image_width) - 15.0f, image_height), ImVec2(0, 0), ImVec2(uv_x, 1.0f));
					// ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 1.0f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

					if (ImGui::ImageButton("Modex::Sidebar::Expand", texture, ImVec2(image_width * (sidebar_w / image_width) - 15.0f, image_height), ImVec2(0, 0), ImVec2(uv_x, 1.0f))) {
						this->expand_sidebar = !this->expand_sidebar;
					}

					// ImGui::PopStyleVar();
					ImGui::PopStyleColor(5);

					// This is an alternative to doing the same UV scaling to the Y axis on a single image.
					// Instead, I just overlay a second image on the first one, which yields the same results.
					if (sidebar_w > min_sidebar_width) {
						ImTextureID overlay = reinterpret_cast<ImTextureID>(GraphicManager::image_library["new_logo_bottom"].texture);
						const float image_alpha = std::clamp(sidebar_w / max_sidebar_width, 0.0f, 1.0f);
						ImGui::SameLine();
						ImGui::SetCursorPos(backup_pos);
						ImGui::SetNextItemAllowOverlap();
						ImGui::Image(overlay, ImVec2(image_width * (sidebar_w / image_width) - 15.0f, image_height), ImVec2(0, 0), ImVec2(uv_x, 1.0f), ImVec4(1, 1, 1, image_alpha));
					}
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				SideBarImage home_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconHome"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage additem_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconAddItem"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage object_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconObject"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage npc_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconNPC"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage teleport_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconTeleport"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage settings_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconSettings"].texture), ImVec2(32.0f, 32.0f) };
				SideBarImage exit_image = { reinterpret_cast<ImTextureID>(GraphicManager::image_library["IconExit"].texture), ImVec2(32.0f, 32.0f) };

				bool is_expanded = false;
				const auto ExpandButton = [&]() {
					// if (ImGui::IsWindowHovered() || ImGui::IsItemHovered()) {
					// 	is_expanded = true;
					// }
					if (this->expand_sidebar) {
						is_expanded = true;
					}
				};

				if (config.showHomeMenu) {
					if (ImGui::SidebarButton("Home", activeWindow == ActiveWindow::Home, home_image.texture, home_image.size, ImVec2(button_width, button_height), home_w)) {
						activeWindow = ActiveWindow::Home;
					}
					ExpandButton();
				}

				if (config.showAddItemMenu) {
					if (ImGui::SidebarButton("Item", activeWindow == ActiveWindow::AddItem, additem_image.texture, additem_image.size, ImVec2(button_width, button_height), additem_w)) {
						activeWindow = ActiveWindow::AddItem;
						if (AddItemWindow::GetSingleton()->GetTableView().GetTableList().empty()) {
							AddItemWindow::GetSingleton()->Refresh();
						}

						AddItemWindow::GetSingleton()->GetTableView().BuildPluginList();
					}

					ExpandButton();
				}

				if (config.showObjectMenu) {
					if (ImGui::SidebarButton("Object", activeWindow == ActiveWindow::Object, object_image.texture, object_image.size, ImVec2(button_width, button_height), object_w)) {
						activeWindow = ActiveWindow::Object;
						if (ObjectWindow::GetSingleton()->GetTableView().GetTableList().empty()) {
							ObjectWindow::GetSingleton()->Refresh();
						}

						NPCWindow::GetSingleton()->GetTableView().BuildPluginList();
					}

					ExpandButton();
				}

				if (config.showNPCMenu) {
					if (ImGui::SidebarButton("NPC", activeWindow == ActiveWindow::NPC, npc_image.texture, npc_image.size, ImVec2(button_width, button_height), npc_w)) {
						activeWindow = ActiveWindow::NPC;
						if (NPCWindow::GetSingleton()->GetTableView().GetTableList().empty()) {
							NPCWindow::GetSingleton()->Refresh();
						}

						NPCWindow::GetSingleton()->GetTableView().BuildPluginList();
					}

					ExpandButton();
				}

				if (config.showTeleportMenu) {
					if (ImGui::SidebarButton("Teleport", activeWindow == ActiveWindow::Teleport, teleport_image.texture, teleport_image.size, ImVec2(button_width, button_height), teleport_w)) {
						activeWindow = ActiveWindow::Teleport;

						TeleportWindow::GetSingleton()->Refresh();
					}

					ExpandButton();
				}

				if (true) {
					if (ImGui::SidebarButton("Papyrus", activeWindow == ActiveWindow::Papyrus, teleport_image.texture, teleport_image.size, ImVec2(button_width, button_height), papyrus_w)) {
						activeWindow = ActiveWindow::Papyrus;
					}

					ExpandButton();
				}

				if (ImGui::SidebarButton("Settings", activeWindow == ActiveWindow::Settings, settings_image.texture, settings_image.size, ImVec2(button_width, button_height), settings_w)) {
					activeWindow = ActiveWindow::Settings;
				}

				ExpandButton();

				if (ImGui::SidebarButton("Exit", false, exit_image.texture, exit_image.size, ImVec2(button_width, button_height), exit_w)) {
					Menu::GetSingleton()->Close();
				}

				ExpandButton();

				if (is_expanded) {
					if (sidebar_w + 20.0f < max_sidebar_width) {
						sidebar_w += 20.0f;
					} else {
						sidebar_w = max_sidebar_width;
					}
				} else {
					if (sidebar_w - 20.0f > min_sidebar_width) {
						sidebar_w -= 20.0f;
					} else {
						sidebar_w = min_sidebar_width;
					}
				}
			}

			ImGui::EndChild();
			ImGui::SameLine();

			// Need to push a clip rect here so that sidebar overlaps contents while maintaining transparency.
			// Otherwise, the overlap will be transparent, and the overlapped content will show under.
			ImVec2 min = { sidebar_w + ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
			ImVec2 max = { min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y };
			ImGui::PushClipRect(min, max, true);

			switch (activeWindow) {
			case ActiveWindow::Home:
				HomeWindow::Draw();
				break;
			case ActiveWindow::AddItem:
				AddItemWindow::GetSingleton()->Draw(sidebar_w);
				break;
			case ActiveWindow::Object:
				ObjectWindow::GetSingleton()->Draw(sidebar_w);
				break;
			case ActiveWindow::NPC:
				NPCWindow::GetSingleton()->Draw(sidebar_w);
				break;
			case ActiveWindow::Teleport:
				TeleportWindow::GetSingleton()->Draw(sidebar_w);
				break;
			case ActiveWindow::Papyrus:
				PapyrusWindow::GetSingleton()->Draw(sidebar_w);
				break;
			case ActiveWindow::Settings:
				SettingsWindow::Draw();
				break;
			default:
				break;
			}

			ImGui::PopClipRect();

			GraphicManager::DrawImage(style.splashImage, ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f));

			ImGui::End();
		}
	}

	void Frame::Install()
	{
		Frame::activeWindow = static_cast<ActiveWindow>(Settings::GetSingleton()->GetConfig().defaultShow);
		this->expand_sidebar = false;
		ResetSelectable();

		// Initalize elements
		AddItemWindow::GetSingleton()->Init(Frame::activeWindow == ActiveWindow::AddItem);
		NPCWindow::GetSingleton()->Init(Frame::activeWindow == ActiveWindow::NPC);
		TeleportWindow::GetSingleton()->Init(Frame::activeWindow == ActiveWindow::Teleport);
		ObjectWindow::GetSingleton()->Init(Frame::activeWindow == ActiveWindow::Object);
		PapyrusWindow::GetSingleton()->Init(Frame::activeWindow == ActiveWindow::Papyrus);

		Blacklist::GetSingleton()->Init();

		HomeWindow::Init();
		SettingsWindow::Init();

		RefreshStyle();
	}

	void Frame::RefreshStyle()
	{
		// AddItemWindow::RefreshStyle();
		// HomeWindow::RefreshStyle();
		// SettingsWindow::RefreshStyle();

		// auto& style = Settings::GetSingleton()->GetStyle();
	}
}  // namespace Modex