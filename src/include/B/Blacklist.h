#pragma once

#include <PCH.h>

namespace Modex
{
	class Blacklist
	{
	public:
		static inline Blacklist* GetSingleton()
		{
			static Blacklist singleton;
			return &singleton;
		}

		void Draw(float a_offset);
	};
}