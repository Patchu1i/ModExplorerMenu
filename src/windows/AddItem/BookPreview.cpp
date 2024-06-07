#include "Utils/Util.h"
#include "Window.h"

void AddItemWindow::ShowBookPreview()
{
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));

	constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
	if (ImGui::Begin("##ReadBookFromAIM", nullptr, flags)) {
		const auto desc = openBook->form->As<RE::TESDescription>();
		RE::BSString buf;
		desc->GetDescription(buf, nullptr);
		std::string bufStr = std::string(buf);
		Utils::RemoveHTMLTags(bufStr);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize(openBook->name).x * 0.5f);
		ImGui::Text(openBook->name);
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
