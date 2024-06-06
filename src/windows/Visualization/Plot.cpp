#include "Window.h"

void VisualizationWindow::ShowPlot(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	ImGui::Text("TBA");

	// auto& io = ImGui::GetIO();
	// ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	// ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
	// constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_Modal;
	// if (ImGui::Begin("Show Histogram Data", nullptr, flags)) {
	// 	ImGui::TextWrapped(
	// 		"This feature is in beta, and it may not work as expected. "
	// 		"Please report any issues or suggestions to the Mod page as feedback. "
	// 		"Considering it's status, using this feature may result in a CTD. "
	// 		"Use at your own risk, and please don't post about crashes in the comments. "
	// 		"Finally: You have to reselect data after picking a new mod file!");

	// 	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	// 	auto modText = selectedMod ? selectedMod->GetFilename().data() : "Filter by mod..";
	// 	if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", modText)) {
	// 		if (ImGui::Selectable("All Mods", selectedMod == nullptr)) {
	// 			selectedMod = nullptr;
	// 			ApplyFilters();
	// 			ImGui::SetItemDefaultFocus();
	// 		}
	// 		for (auto& mod : MEMData::GetModList()) {
	// 			const char* modName = mod->GetFilename().data();
	// 			bool is_selected = false;
	// 			if (ImGui::Selectable(modName, is_selected)) {
	// 				selectedMod = mod;
	// 				ApplyFilters();
	// 			}
	// 			if (is_selected)
	// 				ImGui::SetItemDefaultFocus();
	// 		}
	// 		ImGui::EndCombo();
	// 	}
	// 	ImGui::SameLine();
	// 	auto dataText = histogramData.empty() ? "Select Data" : "Change Data";
	// 	if (ImGui::BeginCombo("##HistogramDataSelection", dataText)) {
	// 		if (ImGui::Selectable("Armor Rating")) {
	// 			histogramData.clear();
	// 			histogramBinCount = 0;
	// 			for (const auto& item : itemList) {
	// 				if (item->formType != RE::FormType::Armor) {
	// 					histogramData.push_back(0.0f);
	// 					continue;
	// 				}

	// 				const auto armorRating = Utils::CalcBaseArmorRating(item->form->As<RE::TESObjectARMO>());

	// 				histogramData.push_back(armorRating);
	// 				histogramBinCount++;
	// 			}
	// 			//std::sort(histogramData.begin(), histogramData.end());
	// 		}
	// 		if (ImGui::Selectable("Gold Value")) {
	// 			histogramData.clear();
	// 			histogramBinCount = 0;
	// 			for (const auto& item : itemList) {
	// 				histogramData.push_back((float)(item->form->GetGoldValue()));
	// 				histogramBinCount++;
	// 			}
	// 			//std::sort(histogramData.begin(), histogramData.end());
	// 		}
	// 		if (ImGui::Selectable("Weight")) {
	// 			histogramData.clear();
	// 			histogramBinCount = 0;
	// 			for (const auto& item : itemList) {
	// 				histogramData.push_back(item->weight);
	// 				histogramBinCount++;
	// 			}
	// 			//std::sort(histogramData.begin(), histogramData.end());
	// 		}
	// 		if (ImGui::Selectable("Base Damage")) {
	// 			histogramData.clear();
	// 			histogramBinCount = 0;
	// 			for (const auto& item : itemList) {
	// 				if (item->formType != RE::FormType::Weapon) {
	// 					histogramData.push_back(0.0f);
	// 					continue;
	// 				}

	// 				const auto baseDamage = Utils::CalcBaseDamage(item->form->As<RE::TESObjectWEAP>());

	// 				histogramData.push_back(baseDamage);
	// 				histogramBinCount++;
	// 			}
	// 			//std::sort(histogramData.begin(), histogramData.end());
	// 		}

	// 		// More to come...

	// 		ImGui::EndCombo();
	// 	}

	// 	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	// 	if (histogramData.empty()) {
	// 		ImGui::Text("No data to display.");

	// 		if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionMax().x, 15.0f))) {
	// 			b_ShowHistogram = false;
	// 		}

	// 		ImGui::End();
	// 		return;
	// 	}

	// 	//auto max = *std::max_element(histogramData.begin(), histogramData.end());
	// 	//auto min = *std::min_element(histogramData.begin(), histogramData.end());
	// 	//ImGui::PlotHistogram("Histogram", histogramData.data(), histogramBinCount, 0, nullptr, min, max, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 20.0f));
	// 	if (ImPlot::BeginPlot("Histogram", nullptr, nullptr, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 20.0f))) {
	// 		ImPlot::SetupAxes("Item", "Value", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
	// 		//ImPlot::PlotBarGroups(ilabels, histogramData.data(), histogramBinCount, 1, 10.0);
	// 		ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 0.0, histogramBinCount);
	// 		ImPlot::PlotBars("Histogram", histogramData.data(), histogramBinCount, 1, 10.0);

	// 		int select = (int)(std::floor(ImPlot::GetPlotMousePos().x));
	// 		if (select < itemList.size()) {
	// 			auto item = itemList.at(select);

	// 			if (ImPlot::IsPlotHovered() && item != nullptr) {
	// 				ShowItemCard(item);
	// 			}
	// 		} else {
	// 			logger::info("Select: {}", select);
	// 		}
	// 		ImPlot::EndPlot();
	// 	}

	// 	// Close if we click outside.
	// 	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
	// 	if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionMax().x, 15.0f))) {
	// 		b_ShowHistogram = false;
	// 	}
	// 	ImGui::PopStyleColor(1);
	// }
	// ImGui::End();
}