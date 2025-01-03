#define DLLEXPORT __declspec(dllexport)

#include "Console.h"
#include "Data.h"
#include "Hooks.h"
#include "Language.h"
#include "Settings.h"
#include "Windows/Frame.h"
#include "Windows/Persistent.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:  // Skypatcher loads here
			logger::info("Data Loaded");
			Modex::PersistentData::LoadFromFile();
			logger::info("Loaded Persistent Data");
			Modex::Language::GetSingleton()->BuildLanguageList();
			logger::info("Finished Building Language List");
			Modex::Settings::GetSingleton()->LoadSettings(Modex::Settings::ini_mem_path);
			Modex::GraphicManager::Init();
			Modex::Settings::GetSingleton()->LoadFont();
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

	Modex::Console::Register();
	//DumpSpecificVersion();

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