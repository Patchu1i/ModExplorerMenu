#include "include/P/Papyrus.h"
#include "extern/PapyrusAPI.h"
#include "include/C/Console.h"
#include "include/G/Graphic.h"
#include "include/U/Util.h"

namespace Modex
{
	// Separate the implementation of the overlay since it requires a different child window.
	// In the future, should probably draw this directly onto a dedicated overlay window / handler.
	void PapyrusWindow::DrawOverlay()
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		constexpr auto overlay_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
		                               ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

		if (ImGui::Begin("PapyrusProfiler Overlay", nullptr, overlay_flags)) {
			this->DrawHistogram(true);
		}

		ImGui::End();
	}

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
						DrawHistogram(false);
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

	void PapyrusWindow::Update()
	{
		if (currentLog.empty() || currentLog == "None") {
			return;
		}

		if (paused) {
			return;
		}

		interval += ImGui::GetIO().DeltaTime;
		if (interval >= 1.0f) {
			this->logReader.UpdateLog(papyrusPath / currentLog.c_str());
			interval = 0.0f;
		}
	}

	void PapyrusWindow::RenderFlameGraph(const FunctionData* func, float xOffset, float yOffset, float a_maxWidth, bool a_rootOnly = false, bool a_overlay = false, float a_opacity = 1.0f)
	{
		const bool flipped = flipY ? true : a_overlay;

		ImGui::SetCursorPos(ImVec2(xOffset, yOffset));
		if (FlameGraphNode(func, a_maxWidth, a_opacity)) {
			root_node = func->id;  // Update the root node if clicked
			this->zoom = 1.0f;     // Reset zoom on click
		}

		if (a_rootOnly) {
			return;
		}

		// This was necessary because by settings some childrens sizes to 1 pixel, it negates the relationship
		// to the maxWidth, thus causing some elements to exceed. Needs to be improved because it will still cause
		// improper scaling / width when we have a lot of narrow elements on a single row (level 1 and 2 of call stacks).

		float total_child_width = 0.0f;

		// Seems like recursion is necessary to draw the children due to the branching tree data structure.
		if (!func->children.empty()) {
			float childXOffset = xOffset;
			float childYOffset = yOffset;

			this->ApplyOffset(flipped, childYOffset);

			for (const auto& [identifier, child] : func->children) {
				float ratio;
				if (func->IsRoot()) {
					ratio = float(child->callCount) / float(func->GetCallCount());
				} else {
					ratio = float(child->GetCallCountAlt()) / float(func->GetCallCountAlt());
				}

				// Horizontally stack children, but adjust sub-zero widths to prevent ImGui auto scaling.

				float spacing = 2.0f;
				float width_offset = (spacing * (func->children.size() - 1));
				float child_width = (a_maxWidth - width_offset) * ratio;

				if (child_width < 1.0f) {
					child_width = 1.0f;
					spacing = 0.0f;
				}

				total_child_width += child_width + spacing;

				if (total_child_width > a_maxWidth + 5.0f) {
					continue;
				}

				RenderFlameGraph(child, childXOffset, childYOffset, child_width, false, a_overlay);
				childXOffset += child_width + spacing;  // Move the X offset for the next child (stack them horizontally with spacing)
			}
		}
	}

	void PapyrusWindow::ApplyOffset(bool a_overlay, float& a_currentOffset)
	{
		if (a_overlay) {
			a_currentOffset -= this->nodeOffset;
		} else {
			a_currentOffset += this->nodeOffset;
		}
	}

	// a_overlay is passed to determine if overlay is drawn externally in Menu.
	void PapyrusWindow::DrawHistogram(bool a_overlay = false)
	{
		bool flipped = flipY ? true : a_overlay;

		if (!a_overlay) {
			ImGui::Checkbox("Show Overlay", &showOverlay);
			ImGui::Checkbox("Flip Y", &flipY);
			ImGui::Checkbox("Full Width", &fullwidth);
			ImGui::SliderInt("Call Threshold", &threshold, 0, 1000);
			ImGui::SliderFloat("Zoom", &zoom, minZoom, maxZoom);

			ImGui::NewLine();

			if (logReader.GetFunctionData().find(root_node) == logReader.GetFunctionData().end()) {
				ImGui::Text("No function data available. %s", root_node.c_str());
				return;
			}
		}

		// Sloppy MVP model here. Need to revisit this to more concretely determine which flags are necessary.
		auto flame_graph_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		if (a_overlay) {
			flame_graph_flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		}

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
			float yOffset;

			if (flipped) {
				yOffset = region.y;
			} else {
				yOffset = 0.0f;
			}

			// Iterate through the parent nodes to build the hierarchy.
			while (parentFunction != nullptr) {
				orderedParentNodes.push_back(parentFunction);
				parentFunction = parentFunction->parent;
			}

			const float padding = ImGui::GetStyle().WindowPadding.x;

			float width;
			if (fullwidth) {
				width = (region.x - (padding * 2.0f)) * zoom;
			} else {
				width = ImGui::GetContentRegionMax().x - padding;  // Adjust for some fixed width
			}

			if (!a_overlay) {
				float minimapStartPos = flipped ? yOffset - this->minimapHeight : yOffset;

				RenderFlameGraphMinimap(&rootFunction, 0.0f, minimapStartPos, width, region);

				if (flipped) {
					yOffset -= this->minimapHeight + (padding * 2.0f);
				} else {
					yOffset += this->minimapHeight + (padding * 2.0f);
				}
			}

			if (!orderedParentNodes.empty()) {
				std::reverse(orderedParentNodes.begin(), orderedParentNodes.end());

				for (const auto& parent : orderedParentNodes) {
					RenderFlameGraph(parent, padding, yOffset, width, true, a_overlay, 0.5f);
					this->ApplyOffset(flipped, yOffset);
				}
			}

			RenderFlameGraph(&rootFunction, padding, yOffset, width, false, a_overlay);
		}

		ImGui::EndChild();
		ImGui::PopStyleColor(4);  // scrollbar
	}

	void PapyrusWindow::RenderFlameGraphMinimap(const FunctionData* func, float /*xOffset*/, float yOffset, float /*width*/, ImVec2 a_region)
	{
		const float padding = ImGui::GetStyle().WindowPadding.x;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 minimapStart = ImGui::GetCursorScreenPos();
		minimapStart.x += ImGui::GetScrollX();
		minimapStart.y += yOffset;
		ImVec2 minimapEnd = ImVec2(minimapStart.x + a_region.x - padding, minimapStart.y + this->minimapHeight);

		// Draw the minimap background
		drawList->AddRectFilled(minimapStart, minimapEnd, IM_COL32(0, 0, 0, 55));

		const float minimapSize = a_region.x - (padding * 2.0f);
		const ImGuiWindow* window = ImGui::GetCurrentWindow();

		ImVec2 previewBoxStart;
		ImVec2 previewBoxEnd;

		if (window->ScrollbarX) {
			const float scroll_max = window->ScrollMax.x;
			const float grab_size = ((a_region.x / (scroll_max + a_region.x)) * minimapSize) + ((padding * 1.0f) / this->zoom);
			previewBoxStart = ImVec2(minimapStart.x + (ImGui::GetScrollX() / this->zoom), minimapStart.y);
			previewBoxEnd = ImVec2(previewBoxStart.x + grab_size + (padding / this->zoom), minimapEnd.y);
			drawList->AddRectFilled(previewBoxStart, previewBoxEnd, IM_COL32(255, 255, 255, 25));
			drawList->AddRect(previewBoxStart, previewBoxEnd, IM_COL32(255, 255, 255, 255));
		}

		if (ImGui::IsMouseHoveringRect(minimapStart, minimapEnd)) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				float dragDelta = ImGui::GetMouseDragDelta(0).x;
				float newScrollX = ImGui::GetScrollX() + (dragDelta * this->zoom);
				ImGui::SetScrollX(newScrollX);
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::ResetMouseDragDelta(0);
			}
		} else {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}

		if (ImGui::GetIO().MouseWheel != 0.0f) {
			this->zoom *= std::pow(1.1f, ImGui::GetIO().MouseWheel);

			if (this->zoom < minZoom) {
				this->zoom = minZoom;
			} else if (this->zoom > maxZoom) {
				this->zoom = maxZoom;
			}
		}

		// Render the flamegraph structure in the minimap
		std::function<void(const FunctionData*, float, float, float)>
			renderMinimapNode;
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
					if (childYOffset + 12.0f > this->minimapHeight) {
						continue;  // Skip rendering if it exceeds the minimap height
					}

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
