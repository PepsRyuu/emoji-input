#include "RegisterShortcut.h"
#include "nativeui/nativeui.h"
#include <Windows.h>

static wchar_t szWindowClass[] = L"emoji_input_app";
std::function<void()> WndProcCallback;


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_HOTKEY:
		WndProcCallback();
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void RegisterShortcut(std::function<void()> callback) {
	WndProcCallback = callback;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;

	WNDPROC proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(szWindowClass, L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowWindow(hWnd, 0);
	UpdateWindow(hWnd);

	RegisterHotKey(hWnd, 0, 8, VK_LBUTTON | VK_OEM_PERIOD); // question mark key combination, I don't get why. 
}
