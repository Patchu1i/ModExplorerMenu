#pragma once

#include "include/D/Data.h"
#include "include/S/Settings.h"

// clang-format off

namespace Modex
{
	class VortexWindow
	{
	public:
        static inline VortexWindow* GetSingleton()
        {
            static VortexWindow singleton;
            return &singleton;
        }

        VortexWindow() = default;
        ~VortexWindow() = default;


		void                Draw(float a_offset);
		void                Init();

        float               fontScale = 1.0f;
	};
}