#pragma once

class Settings
{
public:
	void LoadSettings(const wchar_t* a_path);

	void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);

	static inline Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	[[nodiscard]] static ImVec4 GetColor(std::string& a_str);
	[[nodiscard]] static int GetInt(std::string& a_str);
	[[nodiscard]] static bool GetBool(std::string& a_str);

private:
};