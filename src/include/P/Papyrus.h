#pragma once

#include "include/D/Data.h"
#include "include/L/LogReader.h"
#include "include/S/Settings.h"

namespace Modex
{

	// A copy of the data struct used by PapyrusProfiler.
	class PapyrusProfilerConfig
	{
	public:
		enum class ProfileWriteMode : std::uint32_t
		{
			WriteAtEnd = 0,  // Write all data at once at the end
			WriteLive = 1,   // Continously write data as we collect it
			NoWrite = 2,     // Don't write anything at all (to a file)

			Invalid = 3
		};

		/** If not empty, stacktraces must match at least one of these filters (regex) to be included in collected data. */
		std::vector<std::regex> includeFilters;

		/** Any stacktraces that match any of these filters (regex) are excluded from collected data. */
		std::vector<std::regex> excludeFilters;

		/** 
			 * Filename we want to write our output to (without file extension). 
			 * A suffix and file extension will be added to this. 
			 */
		std::string outFilename = "";

		/** Max suffix we'll add to filepath to avoid overwriting previous outputs (start counting at 0). */
		uint32_t maxFilepathSuffix = 0;

		/** Maximum number of calls we'll collect before we stop profiling. Zero is treated as no limit. */
		uint32_t maxNumCalls = 0;

		/** Maximum number of seconds for which we'll profile. Zero is treated as no limit. */
		uint32_t maxNumSeconds = 0;

		/** Number of calls we want to skip before we actually start recording calls. */
		uint32_t numSkipCalls = 0;

		/** Number of seconds to skip before we actually start recording calls. */
		uint32_t numSkipSeconds = 0;

		/** Show the start / stop debug message box popup. */
		bool showDebugMessageBox = false;

		/** How/when do we want to write data to files? */
		ProfileWriteMode writeMode = ProfileWriteMode::WriteAtEnd;

		/** Did we fail to correctly load this config from a file? */
		// bool failedLoadFromFile = false;

		// static void PopulateConfig(ProfilingConfig& config, const json& jsonData, const std::string& configPath);
	};

	class PapyrusWindow
	{
	public:
		static inline PapyrusWindow* GetSingleton()
		{
			static PapyrusWindow singleton;
			return std::addressof(singleton);
		}

		void Draw(float a_offset);
		void Update();
		void Init(bool a_isDefault);
		bool IsOverlayEnabled() const { return showOverlay; }
		void DrawOverlay();

	private:
		void RenderFlameGraph(const FunctionData* func, float xOffset, float yOffset, float width, bool a_rootOnly, bool a_overlay, float a_opacity);
		void RenderFlameGraphMinimap(const FunctionData* func, float xOffset, float yOffset, float width, ImVec2 a_region);
		void DrawHistogram(bool a_overlay);
		void ApplyOffset(bool a_overlay, float& a_currentOffset);

		std::filesystem::path papyrusPath;
		std::string currentLog;
		float interval;

		LogReader logReader;
		std::string root_node;

		// internal mouse state
		float mouseStartDrag = 0.0f;
		float mouseEndDrag = 0.0f;
		bool isDragging = false;

		enum class LogView
		{
			Histogram
		};

		LogView currentView = LogView::Histogram;

		// logger settings
		int level = 0;
		int threshold = 200;
		bool showOverlay = false;
		bool flipY = false;
		bool paused = false;

		float nodeOffset = 30.0f;
		float minimapHeight = 125.0f;
		bool fullwidth = true;
		float zoom = 1.0f;
		float minZoom = 1.0f;
		float maxZoom = 50.0f;

		static std::string GetViewName(LogView a_view)
		{
			switch (a_view) {
			case LogView::Histogram:
				return "Histogram";
			default:
				return "Unknown";
			}
		}

		static std::filesystem::path FindPapyrusLogDirectory()
		{
			const char* userProfile = std::getenv("USERPROFILE");
			if (!userProfile) {
				logger::error("[PapyrusWindow] USERPROFILE environment variable not found.");
				return "";
			}

			std::filesystem::path papyrusLogPath = std::filesystem::path(userProfile) / "Documents" / "My Games" / "Skyrim.INI" / "SKSE" / "PapyrusProfiler";

			if (!std::filesystem::exists(papyrusLogPath)) {
				logger::error("[PapyrusWindow] Papyrus log directory not found: {}", papyrusLogPath.string());
				return "";
			}

			logger::info("[PapyrusWindow] Found Papyrus log directory: {}", papyrusLogPath.string());

			return papyrusLogPath;
		}
	};
}