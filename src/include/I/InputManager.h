#pragma once
#include "include/M/Menu.h"
#include "include/S/Settings.h"

// clang-format off
namespace Modex
{
    class CharEvent : public RE::InputEvent
	{
	public:
		uint32_t keyCode;  // 18 (ascii code)
	};

	enum : std::uint32_t
	{
		kInvalid = static_cast<std::uint32_t>(-1),
		kKeyboardOffset = 0,
		kMouseOffset = 256,
		kGamepadOffset = 266
	};

    class InputManager
    {
    public:
        static inline InputManager* GetSingleton()
        {
            static InputManager singleton;
            return &singleton;
        }

        InputManager() = default;
        ~InputManager() = default;
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        void        Init();
        void        OnFocusKill();
        void        ProcessInputEvent(RE::InputEvent** a_event);

        // members
        uint32_t       showMenuKey;
        uint32_t       showMenuModifier;
        bool           captureInput;

        bool           shiftDown;
        bool           ctrlDown;
        bool           altDown;
        bool           modifierDown;

        HWND           wndProcHandle;



    };
}
// clang-format on