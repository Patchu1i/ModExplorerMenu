#include "include/H/Hooks.h"
#include "include/I/InputManager.h"
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
	if (a_events) {
		Modex::InputManager::GetSingleton()->ProcessInputEvent(a_events);
	}

	if (Modex::InputManager::GetSingleton()->captureInput) {
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
	};

	struct WndProcHandler_Hook
	{
		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
		{
			switch (a_msg) {
			case WM_KILLFOCUS:
				{
					Modex::InputManager::GetSingleton()->OnFocusKill();
					// set proc handlr?
					break;
				}
			case WM_SETFOCUS:
				{
					// set proc handler?
					break;
				}
			case WM_ACTIVATE:
				{
					if (a_wParam == WA_ACTIVE) {
						// str composition = std::wstring;
					}

					break;
				}

			case WM_IME_NOTIFY:
				{
					switch (a_wParam) {
					case IMN_OPENCANDIDATE:
					case IMN_SETCANDIDATEPOS:
					case IMN_CHANGECANDIDATE:
						Modex::InputManager::GetSingleton()->captureIMEMode = true;
					};

					return S_OK;
				}

			case WM_INPUTLANGCHANGE:
				{
					HKL hkl = (HKL)a_lParam;
					WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
					LCIDToLocaleName(MAKELCID(LOWORD(hkl), SORT_DEFAULT), localeName, LOCALE_NAME_MAX_LENGTH, 0);

					WCHAR lang[9];
					GetLocaleInfoEx(localeName, LOCALE_SISO639LANGNAME2, lang, 9);

					if (wcscmp(lang, L"en_us") == 0) {
						logger::info("[WM_INPUTLANGCHANGE] IME Mode Off");
						Modex::InputManager::GetSingleton()->captureIMEMode = false;
					} else {
						logger::info("[WM_INPUTLANGCHANGE] IME Mode On");
						Modex::InputManager::GetSingleton()->captureIMEMode = true;
					}
					return S_OK;
				}

			case WM_IME_ENDCOMPOSITION:
				{  // Clear candidate list and input contet
					logger::info("[WM_IME_ENDCOMPOSITION] Clear candidate list and input content");
					break;
				}
			case WM_CHAR:
				{
					if (ImGui::GetIO().WantCaptureKeyboard) {
						if (a_wParam == VK_SPACE && GetKeyState(VK_LWIN) < 0) {
							ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_SETFORPROCESS);
							logger::info("[WM_CHAR] ActivateKeyboardLayout");
							return S_OK;
						}

						logger::info("[WM_CHAR] SendUnicodeChar");
						Modex::InputManager::GetSingleton()->SendUnicodeChar(static_cast<uint32_t>(a_wParam));
					}

					return S_OK;
				}
			case WM_IME_SETCONTEXT:
				{
					logger::info("[WM_IME_SETCONTEXT] WndProcHandle Set");
					Modex::InputManager::GetSingleton()->SetWndProcHandle(a_hwnd);
					return DefWindowProc(a_hwnd, a_msg, a_wParam, a_lParam);
				}
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