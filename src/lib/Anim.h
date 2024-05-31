#pragma once
#include "PCH.h"
#include <corecrt_math.h>

class ImGuiAnim
{
public:
	static void Init();
	static void Update();

	static inline float pulse(float speed, float min = 0.0f, float max = 1.0f)
	{
		return min + ((max - min) / 2) + ((max - min) / 2) * (float)sin(speed * ImGui::GetTime());
	}

	static inline float easeIn(float start, float change = 0.5f, float duration = 1.0f)
	{
		float time = (float)ImGui::GetTime();
		float t = time / duration;
		return change * t + start;
	}
};