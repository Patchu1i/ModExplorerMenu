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

		constexpr void Toggle() { enable = !enable; }
		constexpr void Close() { enable = false; }
		constexpr void Open() { enable = true; }

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
		static inline bool is_modifier_pressed = false;
		static inline std::atomic<bool> initialized = false;

	private:
		bool enable = false;
	};

}