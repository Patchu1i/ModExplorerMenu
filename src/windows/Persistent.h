#pragma once

#include <PCH.h>

namespace ModExplorerMenu
{
	class PersistentData
	{
	public:
		void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);
		void LoadSettings(const wchar_t* a_path);
		void SaveSettings();
		void LoadMasterIni(CSimpleIniA& a_ini);
		void LoadThemeFromIni(CSimpleIniA& a_ini);

		void LoadStyleTheme(ImGuiStyle a_theme);

		void CreateDefaultMaster();
		static void FormatMasterIni(CSimpleIniA& a_ini);
		static void FormatThemeIni(CSimpleIniA& a_ini);

		static inline PersistentData* GetSingleton()
		{
			static PersistentData singleton;
			return &singleton;
		}

		constexpr inline static const wchar_t* ini_favorite_path = L"Data/Interface/ModExplorerMenu/User/";
	};
}