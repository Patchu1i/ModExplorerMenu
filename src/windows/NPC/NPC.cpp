#include "NPC.h"

namespace ModExplorerMenu
{
	void NPCWindow::Draw(Settings::Style& a_style, Settings::Config& a_config)
	{
		constexpr auto flags = ImGuiOldColumnFlags_NoResize;
		ImGui::BeginColumns("##HorizontalSplit", 2, flags);

		const float width = ImGui::GetWindowWidth();
		ImGui::SetColumnWidth(0, width * 0.75f);

		// Left Column
		ShowSearch(a_style, a_config);
		ShowFormTable(a_style, a_config);

		// Right Column
		ImGui::NextColumn();
		ShowActions(a_style, a_config);
		ImGui::EndColumns();
	}

	void NPCWindow::Init()
	{
		// auto& config = Settings::GetSingleton()->GetConfig();

		SetState(showAll);

		g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();
		ApplyFilters();

		columnList = NPCColumns();
	}
}