#pragma once
#include "include/S/Settings.h"

// clang-format off
namespace Modex
{
	class Menu
	{
	public:
		void 					Draw();
		void 					Open();
		void 					Close();
		void 					Toggle();

		void 					RefreshStyle();
		void 					RefreshFont();
		void 					OnFocusKill();

		void 					Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
		void 					SyncUserStyleToImGui(Settings::Style user);
		void 					LoadSettings(CSimpleIniA& a_ini);
		void 					ProcessInputEvent(RE::InputEvent** a_event);

		// constructor destructor
		static inline Menu* GetSingleton()
		{
			static Menu singleton;
			return &singleton;
		}

		Menu() = default;
		~Menu() = default;
		Menu(const Menu&) = delete;
		Menu& operator=(const Menu&) = delete;

		// members
		ID3D11Device* 			device;
		ID3D11DeviceContext* 	context;
		ID3D11Device* 			GetDevice() const { return device; };
		ID3D11DeviceContext* 	GetContext() const { return context; };

		bool 					isLoaded;
		bool 					isEnabled;
		bool					pendingFontChange;
		bool 					prevFreezeState;
		bool					showSettingWindow;

		bool 					shiftDown;
		bool 					ctrlDown;
		bool 					altDown;
		bool 					modifierDown;

		HWND 					wndProcHandle;


	private:
		void 					RebuildFontAtlas(); // internal only
	};

}
// clang-format on