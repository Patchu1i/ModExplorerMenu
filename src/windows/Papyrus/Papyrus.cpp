#include "include/P/Papyrus.h"
#include "extern/PapyrusAPI.h"
#include "include/C/Console.h"
#include "include/G/Graphic.h"
#include "include/U/Util.h"

namespace Modex
{
	void PapyrusWindow::Draw(float a_offset)
	{
		float window_padding = ImGui::GetStyle().WindowPadding.y;

		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		ImGui::SetCursorPosX(window_padding + a_offset);

		if (ImGui::BeginChild("##PapyrusWindow::Main", ImVec2(0, 0), true)) {
			ImGui::Text("Checking Path %s", papyrusPath.c_str());
			ImGui::Text("PapyrusProfiler Version: %d", g_PapyrusProfilerInterface->GetBuildNumber());
			if (ImGui::Button("Start Logging")) {
				g_PapyrusProfilerInterface->StartProfilingConfig("ProfileForeverWriteRealtimeDebug.json");
				logReader.ResetData();
			}

			if (ImGui::Button("Stop Logging")) {
				g_PapyrusProfilerInterface->StopProfiling();
			}

			if (ImGui::BeginCombo("Papyrus Log Directory", currentLog.c_str())) {
				if (ImGui::Selectable("None", currentLog == "None")) {
					currentLog = "None";
				}

				for (const auto& entry : std::filesystem::directory_iterator(papyrusPath)) {
					std::string index = entry.path().filename().string();
					if (ImGui::Selectable(index.c_str(), currentLog == index)) {
						currentLog = index;
						logReader.ResetData();
					}
				}

				ImGui::EndCombo();
			}

			if (!currentLog.empty() && currentLog != "None") {
				ImGui::Text("Current Log:");
				ImGui::SameLine();
				ImGui::TextUnformatted(currentLog.c_str());
				ImGui::Text("Update Timer: %.1f", interval);
				ImGui::Text("Last Line Number:");
				ImGui::SameLine();
				ImGui::TextUnformatted(std::to_string(logReader.GetLastPos()).c_str());

				interval += ImGui::GetIO().DeltaTime;
				if (interval >= 1.0f) {
					logReader.UpdateLog(papyrusPath / currentLog.c_str());
					interval = 0.0f;
				}

				if (logReader.GetFunctionData().empty()) {
					ImGui::Text("No function data available.");
				} else {
					if (ImGui::BeginCombo("View", GetViewName(currentView).c_str())) {
						if (ImGui::Selectable("Histogram", currentView == LogView::Histogram)) {
							currentView = LogView::Histogram;
						}

						ImGui::EndCombo();
					}

					ImGui::NewLine();

					switch (currentView) {
					case LogView::Histogram:
						DrawHistogram();
						break;
					}
				}
			} else {
				ImGui::Text("No log selected.");
			}
		}
		ImGui::EndChild();
	}

	bool FlameGraphNode(const FunctionData* a_func, const float a_width, const float a_opacity)
	{
		std::string name;
		if (!a_func->callname.empty()) {
			name = a_func->callname;
		} else {
			name = a_func->name;
		}

		std::string label = name + " (" + std::to_string(a_func->GetCallCount()) + ")";
		std::string unique_id = "##" + a_func->id;

		const float button_opacity = ImGui::GetStyle().Colors[ImGuiCol_Button].w * a_opacity;
		const float text_opacity = ImGui::GetStyle().Colors[ImGuiCol_Text].w * a_opacity;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_func->color.x, a_func->color.y, a_func->color.z, a_func->color.w * button_opacity));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_func->color.x + 0.2f, a_func->color.y + 0.2f, a_func->color.z + 0.2f, a_func->color.w * button_opacity));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, text_opacity));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		ImGui::PushID(unique_id.c_str());
		bool clicked = ImGui::Button(label.c_str(), ImVec2(a_width, 20));
		ImGui::PopID();

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(3);

		return clicked;
	}

	void PapyrusWindow::RenderFlameGraph(const FunctionData* func, float xOffset, float yOffset, float a_maxWidth, bool a_rootOnly = false, float a_opacity = 1.0f)
	{
		ImGui::SetCursorPos(ImVec2(xOffset, yOffset));
		if (FlameGraphNode(func, a_maxWidth, a_opacity)) {
			root_node = func->id;  // Update the root node if clicked
		}

		// if (ImGui::IsItemHovered()) {
		// 	ImGui::BeginTooltip();
		// 	ImGui::TextUnformatted(label.c_str());
		// 	ImGui::Text("Total Calls: %d", func->GetCallCount());
		// 	ImGui::Text("Children:");
		// 	for (const auto& [identifier, child] : func->children) {
		// 		ImGui::Text("%s: %d", child->name.c_str(), child->GetCallCount());
		// 	}
		// 	ImGui::Text("Parent:");
		// 	if (func->parent) {
		// 		ImGui::Text("%s: %d", func->parent->name.c_str(), func->parent->GetCallCount());
		// 	} else {
		// 		ImGui::Text("None");
		// 	}
		// 	ImGui::EndTooltip();
		// }

		if (a_rootOnly) {
			return;
		}

		// Now, recursively render the children of the current function
		if (!func->children.empty()) {
			float childXOffset = xOffset;  // Starting X position for child nodes
			float childYOffset = yOffset + 30.0f;

			for (const auto& [identifier, child] : func->children) {
				float ratio;
				if (func->IsRoot()) {
					ratio = float(child->callCount) / float(func->GetCallCount());
				} else {
					ratio = float(child->GetCallCountAlt()) / float(func->GetCallCountAlt());
				}

				float spacing = 2.0f;
				float width_offset = (spacing * (func->children.size() - 1));
				float child_width = (a_maxWidth - width_offset) * ratio;

				if (child_width < 1.0f) {
					child_width = 1.0f;
					spacing = 0.0f;
				}

				RenderFlameGraph(child, childXOffset, childYOffset, child_width);
				childXOffset += child_width + spacing;  // Move the X offset for the next child (stack them horizontally with spacing)
			}
		}
	}

	void PapyrusWindow::DrawHistogram()
	{
		ImGui::InputInt("Call Stack Level", &level);
		ImGui::SliderInt("Call Threshold", &threshold, 0, 1000);
		ImGui::Checkbox("Full Width", &fullwidth);
		ImGui::SliderFloat("Zoom", &zoom, 1.0f, 5.0f);

		ImGui::NewLine();

		if (logReader.GetFunctionData().find(root_node) == logReader.GetFunctionData().end()) {
			ImGui::Text("No function data available. %s", root_node.c_str());
			return;
		}

		constexpr auto flame_graph_flags = ImGuiWindowFlags_HorizontalScrollbar;

		// Just realized that using the scrollbar size/pos is problematic, this is a workaround.
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		const ImVec2 region = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("##FlameGraph", region, true, flame_graph_flags)) {
			const auto& rootFunction = logReader.GetFunctionData().at(root_node);
			FunctionData* parentFunction = rootFunction.parent;

			std::vector<FunctionData*> orderedParentNodes;
			float yOffset = 0.0f;

			while (parentFunction != nullptr) {
				orderedParentNodes.push_back(parentFunction);
				parentFunction = parentFunction->parent;
			}

			const float padding = ImGui::GetStyle().WindowPadding.x;

			float width;
			if (fullwidth) {
				width = (region.x - padding) * zoom;
			} else {
				width = ImGui::GetContentRegionMax().x - padding;  // Adjust for some fixed width
			}

			RenderFlameGraphMinimap(&rootFunction, padding, yOffset, width, region);
			yOffset += this->minimapHeight + padding;

			if (!orderedParentNodes.empty()) {
				std::reverse(orderedParentNodes.begin(), orderedParentNodes.end());

				for (const auto& parent : orderedParentNodes) {
					RenderFlameGraph(parent, padding, yOffset, width, true, 0.5f);
					yOffset += 30.0f;
				}
			}

			RenderFlameGraph(&rootFunction, padding, yOffset, width - padding);
		}

		ImGui::EndChild();

		ImGui::PopStyleColor(4);  // scrollbar
	}

	void PapyrusWindow::RenderFlameGraphMinimap(const FunctionData* func, float /*xOffset*/, float /*yOffset*/, float /*width*/, ImVec2 a_region)
	{
		// Draw a minimap of the flamegraph
		const float padding = ImGui::GetStyle().WindowPadding.x;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 minimapStart = ImGui::GetCursorScreenPos();
		minimapStart.x += ImGui::GetScrollX();
		ImVec2 minimapEnd = ImVec2(minimapStart.x + a_region.x - padding, minimapStart.y + this->minimapHeight);

		// Draw the minimap background
		drawList->AddRectFilled(minimapStart, minimapEnd, IM_COL32(0, 0, 0, 55));

		// Overlay a rectangle of the currently visible region
		const float minimapSize = a_region.x - (padding * 2.0f);

		// Calculate the size of the grab
		const ImGuiWindow* window = ImGui::GetCurrentWindow();

		ImVec2 previewBoxStart;
		ImVec2 previewBoxEnd;

		if (window->ScrollbarX) {
			const float scroll_max = window->ScrollMax.x;
			const float grab_size = ((a_region.x / (scroll_max + a_region.x)) * minimapSize) + ((padding * 2.0f) / this->zoom);
			previewBoxStart = ImVec2(minimapStart.x + (ImGui::GetScrollX() / this->zoom) - padding, minimapStart.y);
			previewBoxEnd = ImVec2(previewBoxStart.x + grab_size + (padding / this->zoom), minimapEnd.y);
			drawList->AddRectFilled(previewBoxStart, previewBoxEnd, IM_COL32(255, 255, 255, 25));
			drawList->AddRect(previewBoxStart, previewBoxEnd, IM_COL32(255, 255, 255, 255));
		}

		if (ImGui::IsMouseDragPastThreshold(0, 5.0f) && ImGui::IsMouseHoveringRect(previewBoxStart, previewBoxEnd)) {
			float dragDelta = ImGui::GetMouseDragDelta(0).x;
			float newScrollX = ImGui::GetScrollX() + (dragDelta * this->zoom);
			ImGui::SetScrollX(newScrollX);
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::ResetMouseDragDelta(0);
		} else {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}

		// if (ImGui::IsMouseDown(0) && this->isDragging) {
		// 	// float deltaX = this->mouseEndDrag.x - this->mouseStartDrag.x;
		// 	// float newScrollX = ImGui::GetScrollX() + deltaX / this->zoom;

		// 	const ImVec2 mouseStart = ImVec2(this->mouseStartDrag, minimapStart.y);
		// 	const ImVec2 mouseEnd = ImVec2(this->mouseEndDrag, minimapEnd.y);
		// 	drawList->AddRect(mouseStart, mouseEnd, IM_COL32(255, 255, 255, 255));
		// }

		// Render the flamegraph structure in the minimap
		std::function<void(const FunctionData*, float, float, float)> renderMinimapNode;
		renderMinimapNode = [&](const FunctionData* func, float xOffset, float yOffset, float width) {
			ImVec2 nodeStart = ImVec2(minimapStart.x + xOffset, minimapStart.y + yOffset);
			ImVec2 nodeEnd = ImVec2(nodeStart.x + width, nodeStart.y + 10.0f);

			// Draw the node
			drawList->AddRectFilled(nodeStart, nodeEnd, ImGui::ColorConvertFloat4ToU32(func->color));

			// Recursively render children
			if (!func->children.empty()) {
				float childXOffset = xOffset;
				float childYOffset = yOffset + 12.0f;
				for (const auto& [identifier, child] : func->children) {
					float ratio = float(child->GetCallCount()) / float(func->GetCallCount());
					float childWidth = width * ratio;
					renderMinimapNode(child, childXOffset, childYOffset, childWidth);
					childXOffset += childWidth;
				}
			}
		};

		// Start rendering from the root node
		renderMinimapNode(func, 0.0f, 0.0f, minimapSize);
	}

	void PapyrusWindow::Init(bool a_isDefault)
	{
		(void)a_isDefault;  // Unused parameter

		papyrusPath = FindPapyrusLogDirectory();
		currentLog = "None";
		interval = 0.0f;

		logReader = LogReader();
		root_node = "root";

		level = 1;
	}
}
