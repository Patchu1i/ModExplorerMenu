#pragma once

class SettingsWindow
{
public:
	static void Draw();
	static void DrawThemeSelector();
	static void DrawPopped();
	static void Init();

	static inline std::atomic_bool changes = false;
	static inline char savePath[MAX_PATH] = { 0 };
};