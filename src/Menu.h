#pragma once
#include "Settings.h"
#include <regex>

namespace Modex
{
	class Menu
	{
	public:
		void Draw();
		void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
		void SyncUserStyleToImGui(Settings::Style user);
		void LoadSettings(CSimpleIniA& a_ini);
		void ProcessInputEvent(RE::InputEvent** a_event);
		void RefreshStyle();

		bool IsEnabled() { return enable; }
		void SetEnabled(bool value) { enable = value; }

		void Open();
		void Close();

		constexpr void Toggle()
		{
			if (enable) {
				Close();
			} else {
				Open();
			}
		}

		float prevGlobalTimeMultiplier;

		static inline Menu* GetSingleton()
		{
			static Menu singleton;
			return &singleton;
		}

		~Menu();

		ID3D11Device* device;
		ID3D11DeviceContext* context;
		ID3D11Device* GetDevice() const { return device; };
		ID3D11DeviceContext* GetContext() const { return context; };

		static inline bool is_settings_popped = false;
		static inline std::atomic<bool> initialized = false;
		static inline bool _prevFreeze = false;

		static inline bool _isShiftDown = false;
		static inline bool _isCtrlDown = false;
		static inline bool _isAltDown = false;
		static inline bool _isOpenModDown = false;

	private:
		bool enable = false;
	};

}