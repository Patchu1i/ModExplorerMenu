#include "include/H/Hooks.h"
#include "include/M/Menu.h"

decltype(&IDXGISwapChain::Present) ptr_IDXGISwapChain_Present;

HRESULT WINAPI hk_IDXGISwapChain_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags)
{
	Modex::Menu::GetSingleton()->Draw();
	return (This->*ptr_IDXGISwapChain_Present)(SyncInterval, Flags);
}

decltype(&D3D11CreateDeviceAndSwapChain) ptrD3D11CreateDeviceAndSwapChain;

HRESULT WINAPI hk_D3D11CreateDeviceAndSwapChain(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	[[maybe_unused]] const D3D_FEATURE_LEVEL* pFeatureLevels,
	[[maybe_unused]] UINT FeatureLevels,
	UINT SDKVersion,
	const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain** ppSwapChain,
	ID3D11Device** ppDevice,
	[[maybe_unused]] D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext)
{
	logger::info("Upgrading D3D11 feature level to 11.1");

	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;  // Create a device with only the latest feature level

#ifndef NDEBUG
	Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = (*ptrD3D11CreateDeviceAndSwapChain)(
		pAdapter,
		DriverType,
		Software,
		Flags,
		&featureLevel,
		1,
		SDKVersion,
		pSwapChainDesc,
		ppSwapChain,
		ppDevice,
		nullptr,
		ppImmediateContext);

	return hr;
}

static void hk_PollInputDevices(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events);
static inline REL::Relocation<decltype(hk_PollInputDevices)> _InputHandler;  // local

void hk_PollInputDevices(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events)
{
	static RE::InputEvent* dummy[] = { nullptr };
	auto menu = Modex::Menu::GetSingleton();

	if (!a_events) {
		_InputHandler(a_dispatcher, a_events);
		return;
	}

	auto prevState = menu->isEnabled;

	if (menu->isLoaded) {
		menu->ProcessInputEvent(a_events);
	}

	// Small workaround to capture key event on close.
	if (menu->isEnabled || (prevState != menu->isEnabled && prevState == true)) {
		_InputHandler(a_dispatcher, dummy);  // Block Input Events to Skyrim
		return;
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
	};

	struct WndProcHandler_Hook
	{
		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
		{
			const auto& menu = Modex::Menu::GetSingleton();

			switch (a_msg) {
			case WM_KILLFOCUS:
				menu->OnFocusKill();
				// set proc handlr?
				break;

			case WM_SETFOCUS:
				// set proc handler?
				break;

			case WM_ACTIVATE:
				if (a_wParam == WA_ACTIVE) {
					// str composition = std::wstring;
				}

				break;

			case WM_IME_NOTIFY:
				// switch (a_wParam) {
				// case IMN_OPENCANDIDATE:
				// case IMN_SETCANDIDATEPOS:
				// case IMN_CHANGECANDIDATE:
				// 	// update candidate list
				// };

				return S_OK;

			case WM_INPUTLANGCHANGE:
				// check if we are focused on text input
				// if so, enable panel and disable conflicting control maps
				return S_OK;

			case WM_IME_ENDCOMPOSITION:
				// Clear candidate list and input contet
				break;

			case WM_CHAR:
				return S_OK;

			case WM_IME_SETCONTEXT:
				return DefWindowProc(a_hwnd, a_msg, a_wParam, a_lParam);
			}

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

			Modex::Menu::GetSingleton()->Draw();
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		logger::info("Hooking BSInputDeviceManager::PollInputDevices");
		_InputHandler = trampoline.write_call<5>(REL::RelocationID(67315, 68617).address() + REL::Relocate(0x7B, 0x7B, 0x81), hk_PollInputDevices);  //BSInputDeviceManager::PollInputDevices -> Inputfunc

		logger::info("Hooking BSGraphics::Renderer::InitD3D");
		stl::write_thunk_call<D3DInitHook>(REL::RelocationID(75595, 77226).address() + REL::Relocate(0x50, 0x2BC));

		logger::info("Hooking WndProcHandler");
		stl::write_thunk_call_6<RegisterClassA_Hook>(REL::VariantID(75591, 77226, 0xDC4B90).address() + REL::VariantOffset(0x8E, 0x15C, 0x99).offset());

		logger::info("Hooking DXGI Present");
		stl::write_thunk_call<DXGIPresentHook>(REL::RelocationID(75461, 77246).address() + REL::VariantOffset(0x9, 0x9, 0x15).offset());
	}
}