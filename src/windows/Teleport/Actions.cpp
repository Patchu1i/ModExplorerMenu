#include "Console.h"
#include "Teleport.h"

namespace ModExplorerMenu
{
	void TeleportWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		ImGui::SeparatorText("Favorite Locations:");
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const ImVec2 button_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f);
		auto tempList = Data::GetCellMap();
		for (auto& cell : tempList) {
			if (cell.favorite) {
				if (ImGui::Button(cell.GetEditorID().data(), button_size)) {
					Console::Teleport(cell.GetEditorID().data());
					Console::StartProcessThread();
					break;
				}
			}
		}

		ImGui::PopStyleVar(1);
	}
}