#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#pragma once

#ifdef SIMPLE_EXPORTS
#	define SIMPLE_API __declspec(dllexport)
#else
#	define SIMPLE_API __declspec(dllimport)
#endif

namespace SimpleIME
{
	enum class SkseImeMessage
	{
		IME_INTEGRATION_INIT = 0x100,
		IME_COMPOSITION_RESULT,
	};
	
    enum class State : std::uint32_t
    {
        IN_COMPOSING           = 0x1,  // set when IME in composition
        IN_CAND_CHOOSING       = 0x2,  // set when IME in candidate choosing
        IN_ALPHANUMERIC        = 0x4,  // set when input method on english mode
        LANG_PROFILE_ACTIVATED = 0x10, // set when activate any non-english language profile
        IME_DISABLED           = 0x20, // if set, ignore any TextService change
        TSF_FOCUS              = 0x40, // TSF has a valid document focus
    };

	struct SIMPLE_API IntegrationData
	{
		// Other mod must call this to render IME window
		/// Note: If current not exists context, this method will be ignore.
		void (*RenderIme)() = nullptr;

		/// <summary>
		/// Try enable IME.
		/// Must use IsWantCaptureInput to check current state because
		/// IME enabled state is updated asynchronously.
		///
		/// Note: If SimpleIME "Enable mod" unchecked or current not exists context,
		/// this method will be fail(return false)
		/// </summary>
		bool (*EnableIme)(bool enable) = nullptr;

		/// <summary>
		/// Notify SimpleIME a mod menu is open.
		/// SimpleIME needs to use this to disable IME for the game menu
		/// Currently, this method simply increases a reference count that other mods using SimpleIME increment.
		///
		/// Note: PushContext and PopContext must be called in pairs. Whether the "EnableMod" option is checked in
		/// SimpleIME does not affect their execution. These methods should be invoked when a third - party UI mod's
		/// menu opens and closes respectively.
		/// </summary>
		/// <returns>the reference count previously</returns>
		uint32_t (*PushContext)() = nullptr;

		/// <summary>
		/// Notify SimpleIME a mod menu is closed
		/// SimpleIME need to use this to enable IME for game menu.
		/// Currently, this method simply decrease a reference count that other mods using SimpleIME increment.
		///
		/// Note: PushContext and PopContext must be called in pairs. Whether the "EnableMod" option is checked in
		/// SimpleIME does not affect their execution. These methods should be invoked when a third - party UI mod's
		/// menu opens and closes respectively.
		/// </summary>
		/// <returns>the reference count previously</returns>
		uint32_t (*PopContext)() = nullptr;

		// update IME window position (the candidate and composition window)
		// Note: If SimpleIME "Enable mod" unchecked or current not exists context, this method will be ignore.
		void (*UpdateImeWindowPosition)(float posX, float posY) = nullptr;

		/// <summary>
		//  Check current IME want to capture user keyboard input?
		//  Note: iFly won't update conversion mode value
		/// </summary>
		/// <param name="unicodeOrScanCode"> Used to check if the first character input
		/// will trigger the IME when the IME is not input.
		/// </param>
		/// <returns>return true if SimpleIME mod enabled and IME not in alphanumeric mode,
		/// otherwise, return false.
		/// </returns>
        bool (*IsWantCaptureInput)(uint32_t unicodeOrScanCode, bool isUnicode) = nullptr;

		//////////////////////////////////////////////////////////////////////////
		// Optional, These API be used read SimpleIME state
		//////////////////////////////////////////////////////////////////////////

		// Get current state
		// You can use Commonlib-SSE Enumeration API check state flag.
		std::uint32_t (*GetState)() = nullptr;

		// the description equality to input method name.
		std::string (*GetCurrentLangProfileDescription)() = nullptr;
	};

	static_assert(sizeof(IntegrationData) == 0x40);

	class SimpleImeIntegration
	{
		using Message = SKSE::MessagingInterface::Message;

	public:
		enum ImeMessage : std::uint32_t
		{
			IME_INTEGRATION_INIT = 0x100,
			IME_COMPOSITION_RESULT,
		};

		static auto HandleMessage(Message* a_msg) -> void;

		// a global listener to enable IME based on the internal WantTextInput state.
		static void EnableIMEListener()
		{
			auto& instance = SimpleIME::SimpleImeIntegration::GetSingleton();
			if (ImGuiIO io = ImGui::GetIO(); io.WantTextInput) {  // last frame
				instance.EnableIme(true);
			} else {
				instance.EnableIme(false);
			}
		}

		auto EnableIme(bool enable) -> bool;
		auto PushContext() -> uint32_t;
		auto PopContext() -> uint32_t;
		auto RenderIme() -> void;
		auto UpdateImeWindowPosition(float posX, float posY) -> void;

		auto IsWantCaptureInput(uint32_t unicodeOrScanCode, bool isUnicode = true) -> bool
		{
			if (!isIntegrated || integrationData == nullptr) {
				return false;
			}

			return integrationData->IsWantCaptureInput(unicodeOrScanCode, isUnicode);
		}

		static auto GetSingleton() -> SimpleImeIntegration&
		{
			static SimpleImeIntegration g_instance;
			return g_instance;
		}

	private:
		std::atomic_bool isIntegrated = false;
		std::atomic_bool isEnabled = false;
		IntegrationData* integrationData = nullptr;

		auto OnMessageImeIntegrationInit(Message* a_msg) -> void;
		auto OnMessageImeCompositionResult(Message* a_msg) -> void;
	};
}