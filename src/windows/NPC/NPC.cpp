#include "include/N/NPC.h"

namespace Modex
{
	void NPCWindow::Draw()
	{
		constexpr auto flags = ImGuiOldColumnFlags_NoResize;
		ImGui::BeginColumns("##HorizontalSplit", 2, flags);

		const float width = ImGui::GetWindowWidth();
		ImGui::SetColumnWidth(0, width * 0.75f);

		// Left Column
		ShowSearch();
		ShowFormTable();

		// Right Column
		ImGui::NextColumn();
		ShowActions();
		ImGui::EndColumns();
	}

	void NPCWindow::Init()
	{
		g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();
		ApplyFilters();

		columnList = NPCColumns();
	}
}