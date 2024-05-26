#pragma once

#include "PCH.h"
#include <string>
#include <unordered_set>

class MEMData
{
public:
	static inline MEMData* GetSingleton()
	{
		static MEMData singleton;
		return &singleton;
	}

	void Run();

private:
	struct CachedObject  // Removed ItemType (formType), should source from RE::FormType::
	{
		const char* name;
		std::string formid;
		RE::TESForm* form;
		std::string editorid;
		RE::FormType formType;
		std::string typeName;
		std::int32_t goldValue;
		RE::TESFile* mod;
		bool nonPlayable;
		bool favorite;
		int quantity;
		bool selected;
	};

	static inline std::vector<CachedObject> _cache;
	static inline std::vector<RE::TESFile*> _modList;

	template <class T>
	static void CacheItems(RE::TESDataHandler* a_data);
};