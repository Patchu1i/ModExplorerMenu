#pragma once

#include "include/S/Settings.h"
#include <PCH.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace ImGui
{
	[[nodiscard]] inline static const float GetCenterTextPosX(const char* text)
	{
		return ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2 - ImGui::CalcTextSize(text).x / 2;
	};

	// Specifically used in the Frame Sidebar window.
	//
	// @param a_text - The text to display on the button.
	// @param a_texture - The texture to display on the button.
	// @param a_imageSize - The size of the texture.
	// @param a_buttonSize - The size of the button.
	// @param a_textMod - The text alignment modifier for the button.
	static inline bool SidebarButton(const char* a_text, ImTextureID a_texture, ImVec2 a_imageSize, ImVec2 a_buttonSize, float& a_textMod)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(a_text);
		const ImVec2 label_size = ImGui::CalcTextSize(a_text, NULL, true);
		const ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);

		// Set gradient colors manually for now...
		ImU32 bg_color_1 = ImGui::GetColorU32(ImVec4(0.22f, 0.22f, 0.22f, 0.5f));
		ImU32 bg_color_2 = ImGui::GetColorU32(ImVec4(0.22f, 0.22f, 0.22f, 0.5f));

		// Calculate size and bounding box of button element.
		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size =
			ImGui::CalcItemSize(ImVec2(a_buttonSize.x, a_buttonSize.y), label_size.x + style.FramePadding.x * 2.0f,
				label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Emulate button behavior since we're not using ImGui::Button.
		ImGuiButtonFlags flags = ImGuiButtonFlags_None;

		bool hovered, held;
		bool expanded = (a_buttonSize.x) > a_imageSize.x * 4.0f;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		const float anim_step = 0.05f;

		if (hovered || held) {
			if (a_textMod < 0.5f) {
				a_textMod += anim_step;
			} else {
				a_textMod = 0.5f;
			}
		} else {
			if (a_textMod > 0.0f) {
				a_textMod -= anim_step;
			} else {
				a_textMod = 0.0f;
			}
		}

		// This code handles the gradient coloring.
		// https://github.com/ocornut/imgui/issues/4722
		const bool is_gradient = bg_color_1 != bg_color_2;
		if (held || hovered) {
			// Modify colors (ultimately this can be prebaked in the style)
			float h_increase = (held || hovered) ? 0.02f : 0.02f;
			float v_increase = (held || hovered) ? 10.0f : 0.07f;

			ImVec4 bg1f = ImGui::ColorConvertU32ToFloat4(bg_color_1);
			ImGui::ColorConvertRGBtoHSV(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg1f.x = ImMin(bg1f.x + h_increase, 1.0f);
			bg1f.z = ImMin(bg1f.z + v_increase, 1.0f);
			ImGui::ColorConvertHSVtoRGB(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg_color_1 = ImGui::GetColorU32(bg1f);
			if (is_gradient) {
				ImVec4 bg2f = ImGui::ColorConvertU32ToFloat4(bg_color_2);
				ImGui::ColorConvertRGBtoHSV(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg2f.z = ImMin(bg2f.z + h_increase, 1.0f);
				bg2f.z = ImMin(bg2f.z + v_increase, 1.0f);
				ImGui::ColorConvertHSVtoRGB(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg_color_2 = ImGui::GetColorU32(bg2f);
			} else {
				bg_color_2 = bg_color_1;
			}
		}
		ImGui::RenderNavHighlight(bb, id);

		// Calculate image bounding box, relative to the underlying button.
		const ImVec2 image_size = ImVec2((float)a_imageSize.x * 0.80f, (float)a_imageSize.y * 0.80f);
		const float image_pos_height = bb.Min.y + (bb.GetHeight() - image_size.y) * 0.5f;  // always centered.
		const float image_pos_width_min = bb.Min.x + (image_size.x * 0.5f) + style.FramePadding.x;
		const float image_pos_width_max = image_pos_width_min + image_size.x;
		const ImVec2 image_pos_min = ImVec2(image_pos_width_min, image_pos_height);
		const ImVec2 image_pos_max = ImVec2(image_pos_width_max, image_pos_height + image_size.y);

		// Render
		int vert_start_idx = window->DrawList->VtxBuffer.Size;
		window->DrawList->AddImage(a_texture, image_pos_min, image_pos_max);
		window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_color_1, g.Style.FrameRounding);
		int vert_end_idx = window->DrawList->VtxBuffer.Size;
		if (is_gradient)
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vert_start_idx, vert_end_idx,
				bb.Min, bb.GetBL(), bg_color_1, bg_color_2);
		if (g.Style.FrameBorderSize > 0.0f)
			window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_Border),
				g.Style.FrameRounding, 0, g.Style.FrameBorderSize);

		// Custom behavior based on whether the button is in an expanded state or not.
		// This is determined externally by the container this button is in.
		// (If the size of the button > 4x the size of the image, it's considered expanded.)

		if (expanded) {
			ImGui::PushStyleColor(ImGuiCol_Text, text_color);

			ImGui::RenderTextClipped(ImVec2(image_pos_width_min + image_size.x + style.FramePadding.x,
										 bb.Min.y + style.FramePadding.y),
				ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y),
				a_text, NULL, &label_size, ImVec2(a_textMod, 0.5f),  // alignment
				&bb);
			ImGui::PopStyleColor();
		}

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	};

	// Source: https://github.com/ocornut/imgui/issues/319
	// Modified heavily, even though I sort of ended up back at the original code.
	//
	// @param a_id - The unique identifier for the splitter.
	// @param split_vertically - Whether the splitter is vertical or horizontal.
	// @param target - A pointer to the target transform to modify.
	// @param edge - A pointer to the edge which the splitter is bound to (for sizing).
	// @param min_size - The minimum size the target can be.
	// @param max_size - The maximum size the target can be.
	// @param add_ext - An optional pointer to an external transform to modify.
	// @param sub_ext - An optional pointer to an external transform to modify.
	static inline void DrawSplitter(const char* a_id, bool split_vertically, float* target, float* edge, float min_size, float max_size, float* add_ext = nullptr, float* sub_ext = nullptr)
	{
		float width;
		float height;

		if (split_vertically) {
			width = *edge;
			height = 8.0f;
		} else {
			width = 8.0f;
			height = *edge;
		}

		// Override ImGui button styling.
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // We don't draw while active/pressed because as we move the panes the splitter button will be 1 frame late
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.10f));
		ImGui::SetItemAllowOverlap();
		ImGui::Button(a_id, ImVec2(width, height));
		ImGui::PopStyleColor(3);

		if (ImGui::IsItemActive()) {
			float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;

			// Minimum pane size
			if (mouse_delta < min_size - *target)
				mouse_delta = min_size - *target;

			if (mouse_delta > max_size - *target)
				mouse_delta = max_size - *target;

			// Apply resize
			*target += mouse_delta;

			if (add_ext)
				*add_ext += mouse_delta;

			if (sub_ext)
				*sub_ext -= mouse_delta;
		}
	}

	static inline void SubCategoryHeader(const char* label, ImVec4 color = ImVec4(0.22f, 0.22f, 0.22f, 0.5f))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
		ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing()));
		ImGui::PopStyleColor(3);
	}

	// Source: https://github.com/ocornut/imgui/issues/4722
	static inline bool ColoredButtonV1(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		ImGuiButtonFlags flags = ImGuiButtonFlags_None;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const bool is_gradient = bg_color_1 != bg_color_2;
		if (held || hovered) {
			// Modify colors (ultimately this can be prebaked in the style)
			float h_increase = (held || hovered) ? 0.02f : 0.02f;
			float v_increase = (held || hovered) ? 10.0f : 0.07f;

			ImVec4 bg1f = ColorConvertU32ToFloat4(bg_color_1);
			ColorConvertRGBtoHSV(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg1f.x = ImMin(bg1f.x + h_increase, 1.0f);
			bg1f.z = ImMin(bg1f.z + v_increase, 1.0f);
			ColorConvertHSVtoRGB(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg_color_1 = GetColorU32(bg1f);
			if (is_gradient) {
				ImVec4 bg2f = ColorConvertU32ToFloat4(bg_color_2);
				ColorConvertRGBtoHSV(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg2f.z = ImMin(bg2f.z + h_increase, 1.0f);
				bg2f.z = ImMin(bg2f.z + v_increase, 1.0f);
				ColorConvertHSVtoRGB(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg_color_2 = GetColorU32(bg2f);
			} else {
				bg_color_2 = bg_color_1;
			}
		}
		RenderNavHighlight(bb, id);

#if 0
    // V1 : faster but prevents rounding
    window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, bg_color_1, bg_color_1, bg_color_2, bg_color_2);
    if (g.Style.FrameBorderSize > 0.0f)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), 0.0f, 0, g.Style.FrameBorderSize);
#endif

		// V2
		int vert_start_idx = window->DrawList->VtxBuffer.Size;
		window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_color_1, g.Style.FrameRounding);
		int vert_end_idx = window->DrawList->VtxBuffer.Size;
		if (is_gradient)
			ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vert_start_idx, vert_end_idx, bb.Min, bb.GetBL(), bg_color_1, bg_color_2);
		if (g.Style.FrameBorderSize > 0.0f)
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), g.Style.FrameRounding, 0, g.Style.FrameBorderSize);

		if (g.LogEnabled)
			LogSetNextTextDecoration("[", "]");
		PushStyleColor(ImGuiCol_Text, text_color);
		RenderTextClipped(
			ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y),
			ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y),
			label, NULL,
			&label_size,
			style.ButtonTextAlign,
			&bb);
		PopStyleColor();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}

	inline static bool ToggleButton(const char* str_id, bool* v, const float width)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		// float width = height * 3.55f;
		float radius = height * 0.50f;
		bool clicked = false;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked()) {
			*v = !*v;
			clicked = *v;
		}

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))  // && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		ImU32 col_grab;
		if (ImGui::IsItemHovered()) {
			col_bg = ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered), ImVec4(0.40f, 0.70f, 0.40f, 1.0f), t));
			col_grab = ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab), ImVec4(0.78f, 0.78f, 0.78f, 1.0f), t));
		} else {
			col_bg = ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg), ImVec4(0.40f, 0.70f, 0.40f, 1.0f), t));
			col_grab = ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab), ImVec4(0.85f, 0.85f, 0.85f, 1.0f), t));
		}

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, col_grab);

		return clicked;
	}

	inline static void ShowLanguagePopup()
	{
		// auto& style = Modex::Settings::GetSingleton()->GetStyle();

		auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
		auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
		const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
		const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;

		const float pos_x = center_x - (width * 0.5f);
		const float pos_y = center_y - (height * 0.5f);

		const float buttonHeight = ImGui::GetFontSize() * 1.5f;

		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
		if (ImGui::BeginPopupModal("Non-latin Alphabetical Language", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX("Non-latin Alphabetical Language"));
			// ImGui::PushFont(style.font.normal);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
			ImGui::Text("Non-latin Alphabetical Language");
			ImGui::PopStyleColor(1);
			// ImGui::PopFont();

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::TextWrapped(
				"You have selected a non-latin alphabetical language. As a result, you"
				" may need to restart the game for the changes to take effect.");

			ImGui::NewLine();
			ImGui::Text("Do you understand, and wish to proceed?");
			ImGui::NewLine();

			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - (buttonHeight * 2) - 20.0f);  // subtract button size * 2 + separator
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Button("Yes, I understand", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("No, take me back!", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	// TODO: Implement more use cases for this.
	// Call ImGui::OpenPopup(<popup_name>) inside a user event.
	// Call ImGui::ShowWarningPopup(<popup_name>, <callback>) outside of conditional logic.
	inline static void ShowWarningPopup(const char* warning, std::function<void()> callback)
	{
		// auto& style = Modex::Settings::GetSingleton()->GetStyle();

		auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
		auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
		const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
		const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;

		const float pos_x = center_x - (width * 0.5f);
		const float pos_y = center_y - (height * 0.5f);

		const float buttonHeight = ImGui::GetFontSize() * 1.5f;

		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
		if (ImGui::BeginPopupModal(warning, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(warning));
			// ImGui::PushFont(style.font.normal);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
			ImGui::Text(warning);
			ImGui::PopStyleColor(1);
			// ImGui::PopFont();

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::TextWrapped(
				"You are about to spawn a large number of objects. This may result in"
				" undefined behavior such as your game freezing, crashing, or weird things"
				" occuring. It's not recommended to use this outside of testing purposes. ");

			ImGui::NewLine();
			ImGui::Text("Do you understand, and wish to proceed?");
			ImGui::NewLine();

			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - (buttonHeight * 2) - 20.0f);  // subtract button size * 2 + separator
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Button("Yes, I understand", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				callback();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("No, take me back!", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	//
	// ImGui Replacements specifically for theme integration.
	//

	inline static bool GradientButton(const char* label, const ImVec2& size = ImVec2(0, 0))
	{
		auto col_button = ImGui::GetStyle().Colors[ImGuiCol_Button];
		auto col_a = ImGui::GetColorU32(col_button);
		auto col_b = ImGui::GetColorU32(ImVec4(col_button.x * 0.7f, col_button.y * 0.7f, col_button.z * 0.7f, col_button.w));
		return ImGui::ColoredButtonV1(label, size, IM_COL32(255, 255, 255, 255), col_a, col_b);
	}

	inline static bool GradientSelectableEX(const char* label, bool& selected, const ImVec2& size = ImVec2(0, 0))
	{
		auto col_button = selected ? ImGui::GetStyle().Colors[ImGuiCol_Button] : ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		auto alpha = selected ? 1.0f : 0.5f;
		auto col_a = ImGui::GetColorU32(ImVec4(col_button.x, col_button.y, col_button.z, col_button.w * alpha));
		auto col_b = ImGui::GetColorU32(ImVec4(col_button.x * 0.6f, col_button.y * 0.6f, col_button.z * 0.6f, col_button.w * alpha));

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		auto innerPadding = ImGui::GetStyle().FramePadding.y;
		auto newSize = ImVec2(size.x, size.y + innerPadding);
		auto pressed = ImGui::ColoredButtonV1(label, newSize, IM_COL32(255, 255, 255, 255 * alpha), col_a, col_b);
		ImGui::PopStyleVar();

		if (pressed) {
			selected = !selected;
		}

		return pressed;
	}

	// inline static bool m_Selectable(
	// 	const char* label,
	// 	bool& selected,
	// 	Modex::Settings::Style& style,
	// 	ImGuiSelectableFlags flag = ImGuiSelectableFlags_None,
	// 	const ImVec2& size = ImVec2(0, 0))
	// {
	// 	auto innerPadding = style.widgetPadding.y;
	// 	auto newSize = ImVec2(size.x, size.y + innerPadding);

	// 	// ImGui::PushFont(style.buttonFont.normal);
	// 	auto result = ImGui::Selectable(label, &selected, flag, newSize);
	// 	// ImGui::PopFont();
	// 	return result;
	// }

	//
	// End of ImGui Replacements.
	//

	inline static bool DisabledButton(const char* label, bool& disabled, const ImVec2& size = ImVec2(0, 0))
	{
		float alpha = disabled ? 1.0f : 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
		auto result = ImGui::Button(label, size);
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool DisabledCheckbox(const char* label, bool& disabled, bool& v)
	{
		float alpha = disabled ? 0.5f : 1.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);

		bool result;
		if (disabled) {
			auto preview = v;
			result = ImGui::Checkbox(label, &preview);
		} else {
			result = ImGui::Checkbox(label, &v);
		}
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool DisabledImageButton(
		const char* label, bool& disabled, ImTextureID user_texture_id, const ImVec2& size = ImVec2(0, 0),
		const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
	{
		float alpha = disabled ? 0.5f : 1.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
		auto result = ImGui::ImageButton(label, user_texture_id, size, uv0, uv1, bg_col, tint_col);
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool InlineCheckbox(const char* label, bool* v)
	{
		bool changed = false;
		if (ImGui::Checkbox(label, v)) {
			changed = true;
		}
		ImGui::SameLine();
		return changed;
	}

	inline static void SetDelayedTooltip(const char* text, float delay = 1.0f)
	{
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay) {
			if (ImGui::BeginTooltip()) {
				ImGui::Text(text);
				ImGui::EndTooltip();
			}
		}
	}

	inline static void SetQuickTooltip(const char* text)
	{
		if (ImGui::IsItemHovered()) {
			if (ImGui::BeginTooltip()) {
				ImGui::Text(text);
				ImGui::EndTooltip();
			}
		}
	}

	inline static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled(ICON_RPG_QUESTION);
		if (ImGui::BeginItemTooltip()) {
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
	}
}

namespace Utils
{

	inline static std::vector<std::string> GetSkillNames()
	{
		return {
			"Block",
			"Two-Handed",
			"One-Handed",
			"Archery",
			"Light Armor",
			"Pickpocket",
			"Lockpicking",
			"Sneak",
			"Alchemy",
			"Speech",
			"Illusion",
			"Conjuration",
			"Destruction",
			"Restoration",
			"Alteration",
			"Enchanting",
			"Smithing",
			"Heavy Armor"
		};
	}

	// Conversion functions for ANSI <-> UTF-8 <-> UTF-16 (wchar_t) that use std::string and std::wstring
	// Source: https://github.com/Kerite/SkyrimInputMethod/blob/a2e3ba47fe35d000e0eea01b32c666a625cccafd/src/Utils.cpp#L284

	// Convert a wide Unicode string to an UTF8 string
	inline static std::string utf8_encode(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	inline static std::wstring utf8_decode(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	// Convert an wide Unicode string to ANSI string
	inline static std::string unicode2ansi(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an ANSI string to a wide Unicode String
	inline static std::wstring ansi2unicode(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	inline static std::string RemoveQuotesInPath(const std::string& path)
	{
		std::string newPath = path;
		newPath.erase(std::remove(newPath.begin(), newPath.end(), '\"'), newPath.end());
		return newPath;
	}

	inline static void RemoveHTMLTags(std::string& a_string)
	{
		while (a_string.find("<") != std::string::npos) {
			auto startpos = a_string.find("<");
			auto endpos = a_string.find(">") + 1;

			if (endpos != std::string::npos) {
				a_string.erase(startpos, endpos - startpos);
			}
		}

		while (a_string.find("[") != std::string::npos) {
			auto startpos = a_string.find("[");
			auto endpos = a_string.find("]") + 1;

			if (endpos != std::string::npos) {
				a_string.erase(startpos, endpos - startpos);
			}
		}
	};

	static inline std::map<const char*, RE::ActorValue> armorSkillMap = {
		{ "Light Armor", RE::ActorValue::kLightArmor },
		{ "Heavy Armor", RE::ActorValue::kHeavyArmor },
		{ "Clothing", RE::ActorValue::kNone }
	};

	inline static float CalcMaxArmorRating(float a_rating, float a_max)
	{
		return a_rating + a_max;
	};

	// CEILING[ (base armor rating + item quality) × (1 + 0.4 × (skill + skill effect)/100) ] × (1 + unison perk† ) × (1 + Matching Set) × (1 + armor perk‡)
	inline static float CalcBaseArmorRating(RE::TESObjectARMO* a_armor)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		auto armorSkill = a_armor->GetObjectTypeName();
		auto baseRating = a_armor->GetArmorRating();

		if (armorSkillMap[armorSkill] == RE::ActorValue::kNone) {
			return 0;
		}

		auto skill = player->AsActorValueOwner()->GetActorValue(armorSkillMap[armorSkill]);

		return (float)std::ceil((baseRating + 0) * (1.0 + 0.4 * (skill + 0) / 100.0));
	}

	inline static float CalcMaxDamage(float a_damage, float a_max)
	{
		return a_damage + a_max;
	};

	// Round[ (base damage + smithing increase) * (1 + skill/200) * (1 + perk effects) * (1 + item effects) * (1 + potion effect) * (1 + Seeker of Might bonus) ]
	inline static float CalcBaseDamage(RE::TESObjectWEAP* a_weapon)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		auto weaponSkill = a_weapon->weaponData.skill.get();
		auto baseDamage = a_weapon->GetAttackDamage();

		if (weaponSkill == RE::ActorValue::kNone) {
			return baseDamage;
		}

		auto skill = player->AsActorValueOwner()->GetActorValue(weaponSkill);

		return (float)std::round((baseDamage + 0) * (1.0 + skill / 200.0));
	};

	inline static const char* GetSpellType(RE::MagicSystem::SpellType a_type)
	{
		switch (a_type) {
		case RE::MagicSystem::SpellType::kSpell:
			return "Spell";
		case RE::MagicSystem::SpellType::kDisease:
			return "Disease";
		case RE::MagicSystem::SpellType::kPower:
			return "Power";
		case RE::MagicSystem::SpellType::kLesserPower:
			return "Lesser Power";
		case RE::MagicSystem::SpellType::kAbility:
			return "Ability";
		case RE::MagicSystem::SpellType::kPoison:
			return "Poison";
		case RE::MagicSystem::SpellType::kEnchantment:
			return "Enchantment";
		case RE::MagicSystem::SpellType::kPotion:
			return "Potion";
		default:
			return "Unknown";
		}
	}

	inline static const char* GetCastingType(RE::MagicSystem::CastingType a_type)
	{
		switch (a_type) {
		case RE::MagicSystem::CastingType::kConstantEffect:
			return "Constant Effect";
		case RE::MagicSystem::CastingType::kFireAndForget:
			return "Fire and Forget";
		case RE::MagicSystem::CastingType::kConcentration:
			return "Concentration";
		case RE::MagicSystem::CastingType::kScroll:
			return "Scroll";
		default:
			return "Unknown";
		}
	}

	inline static const char* GetDeliveryType(RE::MagicSystem::Delivery a_delivery)
	{
		switch (a_delivery) {
		case RE::MagicSystem::Delivery::kSelf:
			return "Self";
		case RE::MagicSystem::Delivery::kTouch:
			return "Touch";
		case RE::MagicSystem::Delivery::kAimed:
			return "Aimed";
		case RE::MagicSystem::Delivery::kTargetActor:
			return "Target Actor";
		case RE::MagicSystem::Delivery::kTargetLocation:
			return "Target Location";
		default:
			return "Unknown";
		}
	}

	inline static const char* GetArmorType(RE::TESObjectARMO* a_armor)
	{
		switch (a_armor->GetArmorType()) {
		case RE::TESObjectARMO::ArmorType::kLightArmor:
			return "Light Armor";
		case RE::TESObjectARMO::ArmorType::kHeavyArmor:
			return "Heavy Armor";
		case RE::TESObjectARMO::ArmorType::kClothing:
			return "Clothing";
		default:
			return "Unknown";
		};
	};

	inline static const char* GetArmorSlotName(RE::BIPED_MODEL::BipedObjectSlot a_slot)
	{
		switch (a_slot) {
		case RE::BIPED_MODEL::BipedObjectSlot::kAmulet:
			return "35 - Amulet";
		case RE::BIPED_MODEL::BipedObjectSlot::kBody:
			return "32 - Body";
		case RE::BIPED_MODEL::BipedObjectSlot::kCalves:
			return "38 - Calves";
		case RE::BIPED_MODEL::BipedObjectSlot::kCirclet:
			return "42 - Circlet";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitate:
			return "51 - Decapitate";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitateHead:
			return "50 - Decapitate Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kEars:
			return "43 - Ears";
		case RE::BIPED_MODEL::BipedObjectSlot::kFeet:
			return "37 - Feet";
		case RE::BIPED_MODEL::BipedObjectSlot::kForearms:
			return "34 - Forearms";
		case RE::BIPED_MODEL::BipedObjectSlot::kFX01:
			return "61 - FX01";
		case RE::BIPED_MODEL::BipedObjectSlot::kHair:
			return "31 - Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kHands:
			return "33 - Hands";
		case RE::BIPED_MODEL::BipedObjectSlot::kHead:
			return "30 - Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kLongHair:
			return "41 - Long Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmLeft:
			return "58 - Mod Arm Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmRight:
			return "59 - Mod Arm Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModBack:
			return "47 - Mod Back";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestPrimary:
			return "46 - Mod Chest Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestSecondary:
			return "56 - Mod Chest Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModFaceJewelry:
			return "55 - Mod Face Jewelry";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegLeft:
			return "54 - Mod Leg Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegRight:
			return "53 - Mod Leg Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc1:
			return "48 - Mod Misc1";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc2:
			return "60 - Mod Misc2";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMouth:
			return "44 - Mod Mouth";
		case RE::BIPED_MODEL::BipedObjectSlot::kModNeck:
			return "45 - Mod Neck";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisPrimary:
			return "49 - Mod Pelvis Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisSecondary:
			return "52 - Mod Pelvis Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModShoulder:
			return "57 - Mod Shoulder";
		case RE::BIPED_MODEL::BipedObjectSlot::kRing:
			return "36 - Ring";
		case RE::BIPED_MODEL::BipedObjectSlot::kShield:
			return "39 - Shield";
		case RE::BIPED_MODEL::BipedObjectSlot::kTail:
			return "40 - Tail";
		default:
			return "MODEX_ERR";
		};
	};

	// Returns a list of armor slots assigned to the armor.
	// Returns "None" if no slots are assigned to prevent out-of-range access. (Issue #21)
	inline static std::vector<std::string> GetArmorSlots(RE::TESObjectARMO* a_armor)
	{
		std::vector<std::string> slots;
		auto bipedObject = a_armor->As<RE::BGSBipedObjectForm>();
		if (bipedObject) {
			auto slotMask = bipedObject->GetSlotMask();
			for (int i = 0; i < 32; i++) {
				if (static_cast<int>(slotMask) & (1 << i)) {
					slots.push_back(GetArmorSlotName(static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << i)));
				}
			}
		}

		if (slots.empty()) {
			slots.push_back("None");
		}

		return slots;
	}

	template <class T>
	[[nodiscard]] inline static std::string GetItemDescription(RE::TESForm* form, T& a_interface = nullptr)
	{
		std::string s_descFramework = "";
		if (a_interface != nullptr) {
			std::string desc = a_interface->GetDescription(form);
			if (!desc.empty()) {
				Utils::RemoveHTMLTags(desc);
				s_descFramework = std::string(desc) + "\n";
			}
		}

		std::string s_tesDescription = "";
		if (form->As<RE::TESDescription>() != nullptr) {
			const auto desc = form->As<RE::TESDescription>();
			if (desc) {
				RE::BSString buf;
				desc->GetDescription(buf, nullptr);

				if (form->formType == RE::FormType::Book) {
					s_tesDescription = "[Right Click -> Read Me!]";
				} else if (!buf.empty()) {
					s_tesDescription = std::string(buf) + "\n";
				}
			}
		}

		return s_descFramework + s_tesDescription;
	}
}