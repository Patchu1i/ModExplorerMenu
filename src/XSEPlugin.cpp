#define DLLEXPORT __declspec(dllexport)

#include "include/C/Console.h"
#include "include/D/Data.h"
#include "include/F/Frame.h"
#include "include/H/Hooks.h"
#include "include/I/InputManager.h"
#include "include/L/Language.h"
#include "include/P/Persistent.h"
#include "include/S/Settings.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:  // Skypatcher loads here
			Modex::PersistentData::GetSingleton()->LoadAllKits();
			Modex::PersistentData::GetSingleton()->LoadBlacklist();
			logger::info("[kDataLoaded] Loaded Persistent Data");

			Modex::Language::GetSingleton()->BuildLanguageList();
			Modex::FontManager::GetSingleton()->BuildFontLibrary();
			logger::info("[kDataLoaded] Finished Building Language & Font List");

			Modex::Settings::GetSingleton()->LoadSettings(Modex::Settings::ini_mem_path);
			Modex::Settings::GetSingleton()->LoadUserFontSetting();
			Modex::Menu::GetSingleton()->RefreshFont();
			logger::info("[kDataLoaded] Loaded User Settings from ini");

			Modex::GraphicManager::Init();
			Modex::Data::GetSingleton()->Run();
			logger::info("[kDataLoaded] Finished Graphic Manager & Data Initialization");

			Modex::InputManager::GetSingleton()->Init();
			Modex::Frame::GetSingleton()->Install();
			logger::info("[kDataLoaded] Modex has been successfully loaded");
			break;
		case SKSE::MessagingInterface::kPostLoad:
			break;
		case SKSE::MessagingInterface::kPostPostLoad:
			Hooks::Install();
			break;
		case SKSE::MessagingInterface::kPostLoadGame:
			break;
		}
	}
	void SetupLog()
	{
		auto logsFolder = SKSE::log::log_directory();

		if (!logsFolder) {
			SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
		}
		auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
		auto logFilePath = *logsFolder / std::format("{}.log", pluginName);

#ifdef DEBUG
		auto fileLoggerPtr = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
		spdlog::set_default_logger(std::move(loggerPtr));
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);
#else
		auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
		auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
		spdlog::set_default_logger(std::move(loggerPtr));
		spdlog::set_level(spdlog::level::info);
		spdlog::flush_on(spdlog::level::info);
#endif
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SetupLog();
	logger::info("Loaded plugin {} {}", Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	// Install SKSE hooks.
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary();
	v.UsesNoStructs();
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}