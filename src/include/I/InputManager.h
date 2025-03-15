#pragma once
#include "include/K/Keycode.h"
#include "include/M/Menu.h"
#include "include/S/Settings.h"

// clang-format off
namespace Modex
{
    class InputManager
    {
    public:
        static inline InputManager* GetSingleton()
        {
            static InputManager singleton;
            return std::addressof(singleton);
        }

        InputManager() = default;
        ~InputManager() = default;
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        void        Init();
        void        OnFocusChange(bool a_focus);
        void        UpdateSettings();
        void        AddEventToQueue(RE::InputEvent** a_event);
        void        ProcessInputEvents();
        bool        IsBoundModifierDown();

        static inline void ProcessInput() 
        {
            InputManager::GetSingleton()->ProcessInputEvents();
        }

    private:
        // members
        uint32_t       showMenuKey;
        uint32_t       showMenuModifier;
        uint32_t       lastKeyPress;

        bool           shiftDown;
        bool           ctrlDown;
        bool           altDown;
        bool           modifierDown;

        mutable SharedLock _inputLock;
        std::vector<RE::InputEvent*> inputQueue;
    };
}
// clang-format on