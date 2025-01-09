#define DLLEXPORT __declspec(dllexport)

#include "include/C/Console.h"
#include "include/D/Data.h"
#include "include/F/Frame.h"
#include "include/H/Hooks.h"
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
			logger::info("Data Loaded");
			Modex::PersistentData::LoadFromFile();
			Modex::PersistentData::LoadBlacklist();
			logger::info("Loaded Persistent Data");
			Modex::Language::GetSingleton()->BuildLanguageList();
			Modex::FontManager::GetSingleton()->BuildFontLibrary();
			logger::info("Finished Building Language & Font List");
			Modex::Settings::GetSingleton()->LoadSettings(Modex::Settings::ini_mem_path);
			Modex::Settings::GetSingleton()->LoadUserFontSetting();
			Modex::FontManager::GetSingleton()->SetStartupFont();
			Modex::GraphicManager::Init();
			Modex::Data::GetSingleton()->Run();
			Modex::Frame::Install();
			break;
		case SKSE::MessagingInterface::kPostLoad:
			logger::info("kPostLoad");
			break;
		case SKSE::MessagingInterface::kPostPostLoad:
			logger::info("postpostload");
			Hooks::Install();
			break;
		case SKSE::MessagingInterface::kPostLoadGame:
			logger::info("postloadgame");
			break;
		}
	}
	void SetupLog()
	{
		auto logsFolder = SKSE::log::log_directory();
		if (!logsFolder)
			SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
		auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
		auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
		auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
		auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
		spdlog::set_default_logger(std::move(loggerPtr));
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);
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