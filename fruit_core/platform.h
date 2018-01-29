#pragma once

#include <windows.h>

double FruitGetSeconds() {
	static LARGE_INTEGER lastTime;
	static LARGE_INTEGER freq;
	static bool first = true;

	if (first) {
		QueryPerformanceCounter(&lastTime);
		QueryPerformanceFrequency(&freq);

		first = false;
	}

	static double time = 0.0;

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	__int64 delta = t.QuadPart - lastTime.QuadPart;
	double deltaSeconds = double(delta) / double(freq.QuadPart);

	time += deltaSeconds;

	lastTime = t;

	return time;
}