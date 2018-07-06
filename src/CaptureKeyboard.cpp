#include "CaptureKeyboard.h"
#include "nativeui/nativeui.h"
#include <Windows.h>

static HHOOK hook;
static std::function<void(nu::KeyboardCode)> KeyboardProcCallback;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

	if (nCode >= 0 && wParam == WM_KEYDOWN) {
		int vkCode = p->vkCode;

		if (vkCode == VK_ESCAPE) {
			KeyboardProcCallback(nu::KeyboardCode::VKEY_ESCAPE);
			return 1;
		}
		else {
			KeyboardProcCallback(static_cast<nu::KeyboardCode>(vkCode));
			return 1;
		}
	}

	return CallNextHookEx(hook, nCode, wParam, lParam);
}

void StartCapturingKeyboard(std::function<void(nu::KeyboardCode)> callback) {
	KeyboardProcCallback = callback;
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
}

void StopCapturingKeyboard() {
	UnhookWindowsHookEx(hook);
}