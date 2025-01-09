#pragma once

// clang-format off
namespace Modex
{
	class SettingsWindow
	{
	public:
		static void Draw();
		static void DrawGeneralSettings();
		static void DrawBlacklistSettings();
		static void DrawThemeSelector();
		static void DrawTeleportSettings();
		static void DrawAddItemSettings();
		static void DrawPopped();
		static void Init();

		static inline std::atomic_bool changes = false;
		static inline std::atomic_bool file_changes = false;
		static inline char savePath[MAX_PATH] = { 0 };

		static inline int _uiScale = 100;
		static inline int _fontSize = 14;
	};

	// ToDo implement a builder pattern for this class.
	class ISettingManager
	{
	public:
		void 			DrawSettings();

		virtual void 	AddIntSlider(const char* a_text, int& a_value, int a_min, int a_max);
		virtual void 	AddFloatSlider(const char* a_text, float& a_value, float a_min, float a_max);
		virtual void 	AddColorPicker(const char* a_text, ImVec4& a_colRef);
		virtual void 	AddHotkey(const char* a_text, int& a_keybind, int defaultKey);
		virtual void 	AddModifier(const char* a_text, int& a_modifier, int defaultKey);
		virtual void 	AddSelectionDropdown(const char* a_text, int& a_value, std::vector<std::string>& a_options);
	};

}
// clang-format on