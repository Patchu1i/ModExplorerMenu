#pragma once

#include <PCH.h>

// clang-format off
namespace Modex
{

	// https://github.com/Nightfallstorm/DescriptionFramework | License GPL-3.0
	using _GetFormEditorID = const char* (*)(std::uint32_t);
	std::string po3_GetEditorID(RE::FormID a_formID);

	class BaseObject
	{
	public:
		RE::TESForm* TESForm;  // cannot be const
		const RE::FormID FormID;
		const RE::TESFile* TESFile;
		bool favorite = false;

		const std::string name = TESForm->GetName();
		const std::string editorid = po3_GetEditorID(FormID);
		const std::string filename = TESFile->fileName;
		RE::FormID refID = 0;

		// Categorize SoulGem's as Misc.
		[[nodiscard]] inline RE::FormType GetFormType() const
		{
			if (TESForm->GetFormType() == RE::FormType::SoulGem) {
				return RE::FormType::Misc;
			} else {
				return TESForm->GetFormType();
			}
		}

		// TODO: Derefencing char* without safety checks. Danger zone.
		[[nodiscard]] inline std::string GetFormID() const { return fmt::format("{:08x}", TESForm->GetFormID()); }
		[[nodiscard]] inline std::string_view GetName() const { return name; }
		[[nodiscard]] inline std::string_view GetEditorID() const { return editorid; }
		[[nodiscard]] inline RE::FormID GetBaseForm() const { return FormID; }
		[[nodiscard]] inline bool IsFavorite() const { return favorite; }

		[[nodiscard]] inline std::string GetPluginName() const
		{
			if (TESFile != nullptr) {
				return TESFile->fileName;
			} else {
				return "Unknown";
			}
		}

		[[nodiscard]] inline std::string GetTypeName() const
		{
			return static_cast<std::string>(RE::FormTypeToString(TESForm->GetFormType()));
		}
	};

	class StaticObject : public BaseObject
	{
	};

	class Item : public BaseObject
	{
	public:
		const int32_t value = TESForm->GetGoldValue();
		const float weight = TESForm->GetWeight();

		[[nodiscard]] inline float GetWeight() const { return TESForm->GetWeight(); }
		[[nodiscard]] inline std::int32_t GetValue() const { return TESForm->GetGoldValue(); }

		[[nodiscard]] inline std::string GetValueString() const
		{
			return fmt::format("{:d}", TESForm->GetGoldValue());
		}

		[[nodiscard]] inline bool IsNonPlayable() const
		{
			const auto formType = TESForm->GetFormType();
			switch (formType) {
			case RE::FormType::Weapon:
				return TESForm->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
			default:
				return false;
			};
		}
	};

	// TODO: Instead of dereferencing TESForm to retrieve NPC info, should we store it in the NPC class?
	// I took this approach with Items, but I'm not sure if it's the best way to go about it.

	class NPC : public BaseObject
	{
	public:
		// const std::string classname = TESForm->As<RE::TESNPC>()->npcClass->fullName.data();
		RE::TESNPC* AsTESNPC = TESForm->As<RE::TESNPC>();
		const RE::TESRace* race = AsTESNPC->race;
		const bool gender = AsTESNPC->IsFemale();
		const uint16_t level = AsTESNPC->GetLevel();
		const RE::TESClass* npcClass = AsTESNPC->npcClass;
		const RE::BSTArray<RE::FACTION_RANK> factions = AsTESNPC->factions;

		[[nodiscard]] inline std::string_view GetClass() const
		{
			if (npcClass != nullptr) {
				return npcClass->GetFullName();
			} else {
				return "Unknown";
			}
		}

		[[nodiscard]] inline std::string_view GetRace() const
		{
			if (race != nullptr) {
				return race->GetFullName();
			} else {
				return "Unknown";
			}
		}

		[[nodiscard]] inline RE::BSTArray<RE::FACTION_RANK> GetFactions() const
		{
			if (!factions.empty()) {
				return factions;
			} else {
				return {};
			}
		}

		[[nodiscard]] inline uint16_t GetLevel() const { return level; }
		[[nodiscard]] inline std::string GetGender() const { return gender ? "Female" : "Male"; }

		// TODO: Not checking null reference here, danger zone.
		[[nodiscard]] inline float GetHealth() const { return AsTESNPC->GetBaseActorValue(RE::ActorValue::kHealth); }
		[[nodiscard]] inline float GetMagicka() const { return AsTESNPC->GetBaseActorValue(RE::ActorValue::kMagicka); }
		[[nodiscard]] inline float GetStamina() const { return AsTESNPC->GetBaseActorValue(RE::ActorValue::kStamina); }
		[[nodiscard]] inline float GetCarryWeight() const { return AsTESNPC->GetBaseActorValue(RE::ActorValue::kCarryWeight); }
		[[nodiscard]] inline RE::TESNPC::Skills GetSkills() const { return AsTESNPC->playerSkills; }
	};

	class Cell
	{
	public:
		const std::string filename;
		const std::string space;
		const std::string zone;
		const std::string cellName;
		const std::string editorid;

		bool favorite = false;
		const RE::TESFile* mod;

		[[nodiscard]] inline std::string_view GetPluginName() const { return filename; }
		[[nodiscard]] inline std::string_view GetSpace() const { return space; }
		[[nodiscard]] inline std::string_view GetZone() const { return zone; }
		[[nodiscard]] inline std::string_view GetCellName() const { return cellName; }
		[[nodiscard]] inline std::string_view GetEditorID() const { return editorid; }
		[[nodiscard]] inline bool IsFavorite() const { return favorite; }

		Cell(std::string filename, std::string space, std::string zone, std::string cellName, std::string editorid, bool favorite, const RE::TESFile* mod = nullptr) :
			filename(filename), space(space), zone(zone), cellName(cellName), editorid(editorid), favorite(favorite), mod(mod) {}
	};

	class Data
	{
	public:
		static void CacheNPCRefIds();
		static void MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map);

		enum
		{
			ITEM_MOD_LIST = 0,
			NPC_MOD_LIST,
			STATIC_MOD_LIST,
			CELL_MOD_LIST,
			ALL_MOD_LIST
		};

		[[nodiscard]] static inline std::shared_ptr<std::vector<RE::TESFile*>> SortModList(std::shared_ptr<std::vector<RE::TESFile*>> modList, int sort = 0)
		{
			switch (sort) {
			case 0:  // Alphabetical
				std::sort(modList->begin(), modList->end(), [](const RE::TESFile* a, const RE::TESFile* b) {
					if (a == nullptr || b == nullptr) {
						return false;
					}

					if (a->GetFilename().data() == nullptr || b->GetFilename().data() == nullptr) {
						return false;  // Maybe resolves issue #19
					}

					return a->GetFilename().data() < b->GetFilename().data();
				});
				break;
			case 1:  // Last Installed
				std::sort(modList->begin(), modList->end(), [](RE::TESFile* a, RE::TESFile* b) {
					if (a == nullptr || b == nullptr) {
						return false;
					}

					if (_modListLastModified[a] == 0 || _modListLastModified[b] == 0) {
						return false;  // Should never happen (?)
					}

					return _modListLastModified[a] > _modListLastModified[b];
				});
				break;
			}

			return modList;
		}

		// Could Issue #19 be related to lifetime of the returned vector?
		[[nodiscard]] static inline std::shared_ptr<std::vector<RE::TESFile*>> GetModList(int listType, int sort = 0)
		{
			// std::vector<RE::TESFile*>* _unsortedList = std::make_shared<std::vector<RE::TESFile*>>();
			std::shared_ptr<std::vector<RE::TESFile*>> _unsortedList = std::make_shared<std::vector<RE::TESFile*>>();

			switch (listType) {
			case ITEM_MOD_LIST:
				_unsortedList->assign(_itemModList.begin(), _itemModList.end());
				break;
			case NPC_MOD_LIST:
				_unsortedList->assign(_npcModList.begin(), _npcModList.end());
				break;
			case STATIC_MOD_LIST:
				_unsortedList->assign(_staticModList.begin(), _staticModList.end());
				break;
			case CELL_MOD_LIST:
				_unsortedList->assign(_cellModList.begin(), _cellModList.end());
				break;
			default:
				_unsortedList->assign(_modList.begin(), _modList.end());
			}

			return SortModList(_unsortedList, sort);
		}

	private:
		static inline std::vector<Item> _cache;
		static inline std::vector<Cell> _cellCache;
		static inline std::vector<NPC> _npcCache;
		static inline std::vector<RE::TESObjectREFR*> _npcRefIds;
		static inline std::vector<StaticObject> _staticCache;
		static inline std::unordered_set<RE::TESFile*> _modList;

		static inline std::unordered_set<RE::TESFile*> _itemModList;
		static inline std::unordered_set<RE::TESFile*> _npcModList;
		static inline std::unordered_set<RE::TESFile*> _staticModList;
		static inline std::unordered_set<RE::TESFile*> _cellModList;

		static inline std::set<std::string> _npcClassList;
		static inline std::set<std::string> _npcRaceList;
		static inline std::set<std::string> _npcFactionList;

		struct ModFileItemFlags
		{
			bool alchemy = false;
			bool ingredient = false;
			bool ammo = false;
			bool key = false;
			bool misc = false;
			bool armor = false;
			bool book = false;
			bool weapon = false;
			bool scroll = false;
			bool npc = false;
			bool staticObject = false;
			bool tree = false;
			bool activator = false;
			bool container = false;
			bool door = false;
			bool light = false;
			bool furniture = false;
		};

		static inline std::unordered_map<RE::TESFile*, ModFileItemFlags> _itemListModFormTypeMap;
		static inline std::unordered_map<RE::TESFile*, std::time_t> _modListLastModified;

		static void CacheCells(RE::TESFile* a_file, std::vector<Cell>& a_map);

		template <class T>
		static void CacheItems(RE::TESDataHandler* a_data);

		template <class T>
		static void CacheNPCs(RE::TESDataHandler* a_data);

		template <class T>
		static void CacheStaticObjects(RE::TESDataHandler* a_data);

		// Windows Specific API for File Creation Time
		[[nodiscard]] static std::time_t GetFileCreationTime(const std::filesystem::path& path)
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				logger::warn("[Modex] Unable to open file: {}", path.string());
				return std::time_t{};
			}

			FILETIME creationTime;
			if (!GetFileTime(hFile, &creationTime, NULL, NULL)) {
				logger::warn("[Modex] Unable to get file time: {}", path.string());
				CloseHandle(hFile);
				return std::time_t{};
			}

			CloseHandle(hFile);

			ULARGE_INTEGER ull;
			ull.LowPart = creationTime.dwLowDateTime;
			ull.HighPart = creationTime.dwHighDateTime;

			return std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(ull.QuadPart * 100))));
		}

	public:
		static void GenerateItemList();
		static void GenerateNPCList();
		static void GenerateObjectList();
		static void GenerateCellList();

		static void GenerateNPCClassList();
		static void GenerateNPCRaceList();
		static void GenerateNPCFactionList();

		static inline Data* GetSingleton()
		{
			static Data singleton;
			return &singleton;
		}

		[[nodiscard]] static inline std::vector<Cell>& GetCellMap()
		{
			return _cellCache;
		}

		[[nodiscard]] static inline std::vector<Item>& GetItemList()
		{
			return _cache;
		}

		[[nodiscard]] static std::unordered_map<RE::TESFile*, ModFileItemFlags> GetModFormTypeMap()
		{
			return _itemListModFormTypeMap;
		}

		[[nodiscard]] static inline std::vector<NPC>& GetNPCList()
		{
			return _npcCache;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCClassList()
		{
			return _npcClassList;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCRaceList()
		{
			return _npcRaceList;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCFactionList()
		{
			return _npcFactionList;
		}

		[[nodiscard]] static inline std::vector<RE::TESObjectREFR*>& GetNPCRefIds()
		{
			return _npcRefIds;
		}

		[[nodiscard]] static inline std::vector<StaticObject>& GetObjectList()
		{
			return _staticCache;
		}

		void Run();
	};
}
// clang-format on