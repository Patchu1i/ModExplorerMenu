#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
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

	struct SIMPLE_API IntegrationData
	{
		// Other mod must call this to render IME window
		void (*RenderIme)() = nullptr;
		// enable IME for other mod
		bool (*EnableIme)(bool enable) = nullptr;
		// update IME window position (the candidate and composition window)
		void (*UpdateImeWindowPosition)(float posX, float posY) = nullptr;
	};

	static_assert(sizeof(IntegrationData) == 24);

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

		auto EnableIme(bool enable) -> bool;
		auto RenderIme() -> void;
		auto UpdateImeWindowPosition(float posX, float posY) -> void;
		// Update IME window position by last ImGui item position
		auto UpdateImeWindowPosition() -> void;

		auto IsEnabled() -> bool
		{
			return isEnabled.load();
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