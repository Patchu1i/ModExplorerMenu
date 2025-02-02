#pragma once
#include "include/D/DataTypes.h"
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
        void        UpdateSettings();
        void        ProcessInputEvent(RE::InputEvent** a_event);

        // members
        uint32_t       showMenuKey;
        uint32_t       showMenuModifier;
        uint32_t       lastKeyPress;
        bool           captureInput;
        bool           captureIMEMode;

        bool           shiftDown;
        bool           ctrlDown;
        bool           altDown;
        bool           modifierDown;

        HWND           wndProcHandle;

        // Mouse Controller
        // TODO: Set private status
        enum class MOUSE_STATE : std::uint32_t
        {
            kIdle = 0,
            kBoxSelection
        };

        MOUSE_STATE    mouseState;
        ImVec2         mouseSelectionStart;
        ImVec2         mouseSelectionEnd;

        static inline void ClearMouseState() {
            GetSingleton()->mouseState = MOUSE_STATE::kIdle;
            GetSingleton()->mouseSelectionStart = ImVec2(0.0f, 0.0f);
            GetSingleton()->mouseSelectionEnd = ImVec2(0.0f, 0.0f);
        }

        static inline MOUSE_STATE GetMouseState() { return GetSingleton()->mouseState; }
        static inline void SetMouseState(MOUSE_STATE a_state) { GetSingleton()->mouseState = a_state; }
        static inline ImVec2 GetMouseSelectionStart() { return GetSingleton()->mouseSelectionStart; }
        static inline ImVec2 GetMouseSelectionEnd() { return GetSingleton()->mouseSelectionEnd; }
        static inline void SetMouseSelectionStart(ImVec2 a_pos) { GetSingleton()->mouseSelectionStart = a_pos; }
        static inline void SetMouseSelectionEnd(ImVec2 a_pos) { GetSingleton()->mouseSelectionEnd = a_pos; }
        
        // Kit MOuse Controller
        // TODO: Need to create some kind of event system or observer pattern for this.
        MOUSE_STATE     mouseKitState;
        ImVec2          mouseKitSelectionStart;
        ImVec2          mouseKitSelectionEnd;

        static inline void ClearMouseKitState() {
            GetSingleton()->mouseKitState = MOUSE_STATE::kIdle;
            GetSingleton()->mouseKitSelectionStart = ImVec2(0.0f, 0.0f);
            GetSingleton()->mouseKitSelectionEnd = ImVec2(0.0f, 0.0f);
        }

        static inline MOUSE_STATE GetMouseKitState() { return GetSingleton()->mouseKitState; }
        static inline void SetMouseKitState(MOUSE_STATE a_state) { GetSingleton()->mouseKitState = a_state; }
        static inline ImVec2 GetMouseKitSelectionStart() { return GetSingleton()->mouseKitSelectionStart; }
        static inline ImVec2 GetMouseKitSelectionEnd() { return GetSingleton()->mouseKitSelectionEnd; }
        static inline void SetMouseKitSelectionStart(ImVec2 a_pos) { GetSingleton()->mouseKitSelectionStart = a_pos; }
        static inline void SetMouseKitSelectionEnd(ImVec2 a_pos) { GetSingleton()->mouseKitSelectionEnd = a_pos; }

    };
}
// clang-format on