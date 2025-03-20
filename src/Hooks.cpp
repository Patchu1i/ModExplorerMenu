#include "include/H/Hooks.h"
#include "include/I/InputManager.h"
#include "include/M/Menu.h"

static void hk_PollInputDevices(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events);
static inline REL::Relocation<decltype(hk_PollInputDevices)> _InputHandler;  // local

// Note to self: It's important that we read and process all events from the engine. When applying any sort of
// conditional logic, we become suseptible to bugs caused by press/release events not being matched.

void hk_PollInputDevices(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events)
{
	if (a_events) {
		Modex::InputManager::GetSingleton()->AddEventToQueue(a_events);
	}

	if (Modex::Menu::IsEnabled()) {
		static RE::InputEvent* dummy[] = { nullptr };
		_InputHandler(a_dispatcher, dummy);
	} else {
		_InputHandler(a_dispatcher, a_events);
	}
}

namespace Hooks
{
	struct D3DInitHook
	{
		static void thunk()
		{
			func();

			auto manager = RE::BSGraphics::Renderer::GetSingleton();
			auto context = reinterpret_cast<ID3D11DeviceContext*>(manager->GetRuntimeData().context);
			auto swapchain = reinterpret_cast<IDXGISwapChain*>(manager->GetRuntimeData().renderWindows->swapChain);
			auto device = reinterpret_cast<ID3D11Device*>(manager->GetRuntimeData().forwarder);

			Modex::Menu::GetSingleton()->Init(swapchain, device, context);
		}

		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::atomic<bool> init{ false };
	};

	struct WndProcHandler_Hook
	{
		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
		{
			switch (a_msg) {
			case WM_SETFOCUS:
				Modex::InputManager::GetSingleton()->OnFocusChange(true);  // TODO: On Focus Change
				break;
			case WM_KILLFOCUS:
				Modex::InputManager::GetSingleton()->OnFocusChange(false);
				break;
			}

			// TODO: IME Implementation Here.

			return func(a_hwnd, a_msg, a_wParam, a_lParam);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct RegisterClassA_Hook
	{
		static ATOM thunk(WNDCLASSA* a_wndClass)
		{
			WndProcHandler_Hook::func = reinterpret_cast<uintptr_t>(a_wndClass->lpfnWndProc);
			a_wndClass->lpfnWndProc = &WndProcHandler_Hook::thunk;

			return func(a_wndClass);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct DXGIPresentHook
	{
		static void thunk(std::uint32_t a_p1)
		{
			func(a_p1);

			Modex::Menu::GetSingleton()->Update();
			Modex::Menu::GetSingleton()->Draw();
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		logger::info("[Hook] Hooking BSInputDeviceManager::PollInputDevices");
		_InputHandler = trampoline.write_call<5>(REL::RelocationID(67315, 68617).address() + REL::Relocate(0x7B, 0x7B, 0x81), hk_PollInputDevices);  //BSInputDeviceManager::PollInputDevices -> Inputfunc

		logger::info("[Hook] Hooking BSGraphics::Renderer::InitD3D");
		stl::write_thunk_call<D3DInitHook>(REL::RelocationID(75595, 77226).address() + REL::Relocate(0x50, 0x2BC));
		D3DInitHook::init.store(true);

		logger::info("[Hook] Hooking WndProcHandler");
		stl::write_thunk_call_6<RegisterClassA_Hook>(REL::VariantID(75591, 77226, 0xDC4B90).address() + REL::VariantOffset(0x8E, 0x15C, 0x99).offset());

		logger::info("[Hook] Hooking DXGI Present");
		stl::write_thunk_call<DXGIPresentHook>(REL::RelocationID(75461, 77246).address() + REL::VariantOffset(0x9, 0x9, 0x15).offset());
	}
}