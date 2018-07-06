#include "HideFromTaskbar.h"
#include "nativeui/nativeui.h"
#include "nativeui/win/window_win.h"
#include <Windows.h>

void HideFromTaskbar(nu::Window* window) {
	auto hwnd = window->GetNative()->hwnd();
	SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
}