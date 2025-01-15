#include "include/A/AddItem.h"

namespace Modex
{
	void AddItemWindow::ShowBookPreview()
	{
		auto& io = ImGui::GetIO();
		auto& config = Settings::GetSingleton()->GetConfig();
		ImVec2 displaySize = io.DisplaySize;
		displaySize.x *= config.screenScaleRatio.x;
		displaySize.y *= config.screenScaleRatio.y;

		ImGui::SetNextWindowPos(ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f));

		constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
		if (ImGui::Begin("##ReadBookFromAIM", nullptr, flags)) {
			auto desc = const_cast<RE::TESDescription*>(openBook->GetForm()->As<RE::TESDescription>());
			RE::BSString buf;
			desc->GetDescription(buf, nullptr);
			std::string bufStr = std::string(buf);
			Utils::RemoveHTMLTags(bufStr);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize(openBook->GetNameView().data()).x * 0.5f);
			ImGui::Text(openBook->GetNameView().data());
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