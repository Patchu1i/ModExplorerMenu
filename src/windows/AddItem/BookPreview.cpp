#include "AddItem.h"
#include "Utils/Util.h"


namespace ModExplorerMenu
{
	void AddItemWindow::ShowBookPreview()
	{
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));

		constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
		if (ImGui::Begin("##ReadBookFromAIM", nullptr, flags)) {
			auto desc = const_cast<RE::TESDescription*>(openBook->TESForm->As<RE::TESDescription>());
			RE::BSString buf;
			desc->GetDescription(buf, nullptr);
			std::string bufStr = std::string(buf);
			Utils::RemoveHTMLTags(bufStr);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize(openBook->GetName().data()).x * 0.5f);
			ImGui::Text(openBook->GetName().data());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			ImGui::TextWrapped(bufStr.c_str());
		}

		// Close if we click outside.
		if (ImGui::IsMouseClicked(0, true)) {
			if (!ImGui::IsWindowHovered()) {
				openBook = nullptr;
			}
		}

		ImGui::End();
	}
}