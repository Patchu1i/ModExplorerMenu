#define DLLEXPORT __declspec(dllexport)

#include "Console.h"
#include "Data.h"
#include "Frame.h"
#include "Hooks.h"
#include <spdlog/sinks/basic_file_sink.h>

//#include "versiondb.h"
#include "versionlibdb.h"

// for SE versions
//bool DumpSpecificVersion()
//{
//	VersionDb db;
//
//	// Try to load database of version 1.5.62.0 regardless of running executable version.
//	if (!db.Load(1, 5, 97, 0)) {
//		logger::info("Failed to load database for 1.5.97.0!");
//		return false;
//	}
//
//	// Write out a file called offsets-1.5.62.0.txt where each line is the ID and offset.
//	db.Dump("offsets-1.5.97.0.txt");
//	logger::info("Dumped offsets for 1.5.97.0");
//	return true;
//}

//void DumpSpecificVersion()
//{
//VersionDb db;

// Try to load database of version 1.5.62.0 regardless of running executable version.
//if (!db.Load(1, 6, 1170, 0)) {
//	logger::info("Failed to load database for 1.6.1170.0!");
//	return false;
//}

// Write out a file called offsets-1.5.62.0.txt where each line is the ID and offset.
//db.Dump("offsets-1.6.1170.txt");
//logger::info("Dumped offsets for 1.6.1170.0");
//return true;

//void* test_address = db.FindAddressById(67315);

//}

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			MEMData::GetSingleton()->Run();
			Frame::Install();
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

	ConsoleCommand::Register();
	//DumpSpecificVersion();

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}