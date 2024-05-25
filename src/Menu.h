#pragma once

class Menu
{
public:

	void Draw();
	void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
	void ProcessInputEvent(RE::InputEvent** a_event);

	bool IsEnabled() { return enable; }
	void SetEnabled(bool value) { enable = value; }
	void Toggle() { enable = !enable; }

	static inline Menu* GetSingleton()
	{
		static Menu singleton;
		return &singleton;
	}

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Device* GetDevice() const { return device; };
	ID3D11DeviceContext* GetContext() const { return context; };

	static constexpr inline uint32_t keybind = 210;
    static inline std::atomic<bool> initialized = false;


private:

	bool enable = false;

};