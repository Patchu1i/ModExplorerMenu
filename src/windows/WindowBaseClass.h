#pragma once

#include <PCH.h>

struct WindowBaseClass
{
    virtual void Draw() = 0;
    virtual void Init() = 0;

    std::string Name = "Default";
    bool Open = false;

    ImFont *font_test;

    enum WindowType
    {
        window_homepage,
        window_additem,
        window_lookup,
        window_teleport,
        window_settings
    };

    WindowType Type;

};
