#pragma once

#include "include/K/Keycode.h"
#include "include/S/Settings.h"
#include "include/U/Util.h"
#include <PCH.h>

namespace Modex
{
	class UIManager
	{
	public:
		static inline UIManager* GetSingleton()
		{
			static UIManager singleton;
			return std::addressof(singleton);
		}

		void ShowWarning(const std::string& message, std::function<void()> onConfirmCallback)
		{
			m_pendingWarningMessage = message;
			m_onConfirmCallback = onConfirmCallback;
		}

		void ShowHotkey(uint32_t* a_hotkey, const uint32_t& a_default, bool a_modifierOnly, std::function<void()> onConfirmHotkeyCallback)
		{
			m_hotkeyCurrent = a_hotkey;
			m_hotkeyDefault = a_default;
			m_modifierOnly = a_modifierOnly;
			m_onConfirmHotkeyCallback = onConfirmHotkeyCallback;
		}

		bool IsWarningPending() const
		{
			return !m_pendingWarningMessage.empty();
		}

		bool IsHotkeyPending() const
		{
			return m_onConfirmHotkeyCallback != nullptr;
		}

		void AcceptWarning()
		{
			if (m_onConfirmCallback) {
				m_onConfirmCallback();
			}

			m_pendingWarningMessage.clear();
			m_onConfirmCallback = nullptr;
		}

		void DeclineWarning()
		{
			m_pendingWarningMessage.clear();
			m_onConfirmCallback = nullptr;
		}

		void AcceptHotkey()
		{
			// auto& config = Settings::GetSingleton()->GetConfig();
			// config.showMenuKey = m_hotkeyCurrent;

			if (m_onConfirmHotkeyCallback) {
				m_onConfirmHotkeyCallback();
			}

			m_hotkeyCurrent = 0;
			m_hotkeyDefault = 0;
			m_onConfirmHotkeyCallback = nullptr;
		}

		void DeclineHotkey()
		{
			m_hotkeyCurrent = 0;
			m_hotkeyDefault = 0;
			m_onConfirmHotkeyCallback = nullptr;
		}

		// Called from InputManager with IsDown() == true;
		// Return true to prevent handling ESC key.
		bool InputHandler(ImGuiKey a_key)
		{
			if (IsWarningPending()) {
				if (a_key == ImGuiKey_UpArrow || a_key == ImGuiKey_DownArrow) {
					nav_accept = !nav_accept;  // lol
				}

				if (a_key == ImGuiKey_Enter) {
					if (nav_accept) {
						AcceptWarning();
					} else {
						DeclineWarning();
					}
					return false;

				} else if (a_key == ImGuiKey_Escape) {
					DeclineWarning();
					return true;

				} else if (a_key == ImGuiKey_Y) {
					AcceptWarning();
					return false;

				} else if (a_key == ImGuiKey_N) {
					DeclineWarning();
					return false;
				}
			}

			if (IsHotkeyPending()) {
				if (a_key == ImGuiKey_T) {
					*m_hotkeyCurrent = m_hotkeyDefault;  // handle in callback as default
					AcceptHotkey();
					return false;
				}

				if (a_key == ImGuiKey_Escape) {
					DeclineHotkey();
					return true;
				}

				if (ImGui::ImGuiKeyToSkyrimKey(a_key) == *m_hotkeyCurrent) {
					DeclineHotkey();
					return true;
				}

				if (!m_modifierOnly) {
					if (ImGui::IsKeyboardWhitelist(a_key)) {
						*m_hotkeyCurrent = ImGui::ImGuiKeyToSkyrimKey(a_key);
						AcceptHotkey();
						return true;
					}
				} else {
					if (ImGui::IsKeyboardModifier(a_key)) {
						*m_hotkeyCurrent = ImGui::ImGuiKeyToSkyrimKey(a_key);
						AcceptHotkey();
						return true;
					}
				}
			}

			return false;
		}

		void Draw()
		{
			if (IsWarningPending()) {
				_warning();
			}

			if (IsHotkeyPending()) {
				_hotkey();
			}
		}

	private:
		UIManager() = default;
		~UIManager() = default;

		bool nav_accept = true;

		std::string m_pendingWarningMessage;
		std::function<void()> m_onConfirmCallback;

		bool m_modifierOnly;
		uint32_t m_hotkeyDefault;
		uint32_t* m_hotkeyCurrent = nullptr;
		std::function<void()> m_onConfirmHotkeyCallback;

		void _hotkey()
		{
			ImGui::OpenPopup("Modex::Hotkey");

			// All this bullshit is to center the modal window, hopefully..
			auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
			auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
			const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
			const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;
			const float pos_x = center_x - (width * 0.5f);
			const float pos_y = center_y - (height * 0.5f);
			// const float buttonHeight = ImGui::GetFontSize() * 1.5f;

			ImGui::SetNextWindowSize(ImVec2(width, height));
			ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
			if (ImGui::BeginPopupModal("Modex::Hotkey", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
				if (ImGui::IsWindowAppearing()) {  // Needed still (?)
					ImGui::GetIO().ClearInputKeys();
				}

				ImGui::SubCategoryHeader("Set a Hotkey", ImVec4(0.20f, 0.20f, 0.20f, 1.0f));

				ImGui::NewLine();

				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(_T("CONFIG_HOTKEY_SET")));
				ImGui::Text(_T("CONFIG_HOTKEY_SET"));
				ImGui::NewLine();

				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(_T("CONFIG_HOTKEY_RESET")));
				ImGui::Text(_T("CONFIG_KEY_RESET"));
				ImGui::NewLine();

				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(_T("CONFIG_KEY_CANCEL")));
				ImGui::Text(_T("CONFIG_KEY_CANCEL"));

				ImGui::SetCursorPosY(ImGui::GetWindowSize().y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
				if (ImGui::GradientButton(_T("Close"), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing()))) {
					DeclineHotkey();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void _warning()
		{
			ImGui::OpenPopup("Modex::Warning");

			// All this bullshit is to center the modal window, hopefully..
			auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
			auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
			const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
			const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;
			const float pos_x = center_x - (width * 0.5f);
			const float pos_y = center_y - (height * 0.5f);
			const float buttonHeight = ImGui::GetFontSize() * 1.5f;

			ImGui::SetNextWindowSize(ImVec2(width, height));
			ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
			if (ImGui::BeginPopupModal("Modex::Warning", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
				ImGui::SubCategoryHeader("! Warning !", ImVec4(0.35f, 0.20f, 0.20f, 1.0f));

				ImGui::TextWrapped(m_pendingWarningMessage.c_str());

				ImGui::SetCursorPosY(ImGui::GetWindowSize().y - (buttonHeight * 2) - 20.0f);  // subtract button size * 2 + separator
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				const ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
				ImGui::PushStyleColor(ImGuiCol_Button, nav_accept ? buttonColor : ImGui::GetStyleColorVec4(ImGuiCol_Button));
				if (ImGui::GradientButton("(Y)es", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
					AcceptWarning();

					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Button, nav_accept ? ImGui::GetStyleColorVec4(ImGuiCol_Button) : buttonColor);
				if (ImGui::GradientButton("(N)o", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
					DeclineWarning();

					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleColor();

				ImGui::EndPopup();
			}
		}
	};
}