#pragma once

#include <Windows.h>
typedef HWND FOREGROUND_ID;

struct Point {
	int x;
	int y;
};

FOREGROUND_ID ForegroundWindowGet();
void ForegroundWindowSet(FOREGROUND_ID);
Point ForegroundPositionGet(FOREGROUND_ID);