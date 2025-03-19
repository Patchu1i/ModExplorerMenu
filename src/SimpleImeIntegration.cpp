#include "include/S/SimpleImeIntegration.h"
#include "imgui.h"

namespace SimpleIME
{
	auto SimpleImeIntegration::HandleMessage(SKSE::MessagingInterface::Message* a_msg) -> void
	{
		auto& instance = GetSingleton();
		switch (a_msg->type) {
		case IME_INTEGRATION_INIT:
			instance.OnMessageImeIntegrationInit(a_msg);
			break;
		case IME_COMPOSITION_RESULT:
			instance.OnMessageImeCompositionResult(a_msg);
			break;
		default:
			break;
		}
	}

	auto SimpleImeIntegration::EnableIme(bool enable) -> bool
	{
		if (!isIntegrated || integrationData == nullptr) {
			isEnabled.store(false);
			return false;
		}
		if (integrationData->EnableIme(enable)) {
			isEnabled.store(enable);
			return true;
		}
		return false;
	}

	auto SimpleImeIntegration::RenderIme() -> void
	{
		if (!isIntegrated || integrationData == nullptr || !isEnabled) {
			return;
		}

		integrationData->RenderIme();
	}

	auto SimpleImeIntegration::UpdateImeWindowPosition(float posX, float posY) -> void
	{
		if (!isIntegrated || integrationData == nullptr || !isEnabled) {
			return;
		}

		integrationData->UpdateImeWindowPosition(posX, posY);
	}

	auto SimpleImeIntegration::UpdateImeWindowPosition() -> void
	{
		auto min = ImGui::GetItemRectMin();
		auto max = ImGui::GetItemRectMax();
		UpdateImeWindowPosition(min.x, max.y);
	}

	auto SimpleImeIntegration::OnMessageImeIntegrationInit(Message* a_msg) -> void
	{
		logger::debug("SimpleIme: Received message IME_INTEGRATION_INIT");
		if (isIntegrated.load()) {
			logger::warn("SimpleIme: already integrated, message will be ignore");
			return;
		}

		if (a_msg->data != nullptr && a_msg->dataLen == sizeof(IntegrationData*)) {
			integrationData = reinterpret_cast<IntegrationData*>(a_msg->data);
			isIntegrated.store(true);
			logger::debug("SimpleIme: Integration successful!");
		}
	}

	// wchar_t ptr, dataLen is bytes length
	auto SimpleImeIntegration::OnMessageImeCompositionResult(Message* a_msg) -> void
	{
		if (!isIntegrated) {
			return;
		}

		if (a_msg->data != nullptr) {
			logger::debug("SimpleIme: Received result string!");

			auto* wcharPtr = reinterpret_cast<wchar_t*>(a_msg->data);
			std::wstring resultString(wcharPtr, a_msg->dataLen / sizeof(wchar_t));

			auto& io = ImGui::GetIO();
			for (uint32_t const code : resultString) {
				io.AddInputCharacter(code);
			}
		}
	}
}