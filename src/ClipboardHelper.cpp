#include "ClipboardHelper.h"
#include "ForegroundWindow.h"
#include <stdio.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include "nativeui/nativeui.h"

// https://support.microsoft.com/en-us/help/274308/how-to-add-html-code-to-the-clipboard-by-using-visual-c
bool CopyToClipboard(std::string html, std::string plain) {
	// Create temporary buffer for HTML header...
	char *buf = new char[400 + strlen(html.c_str())];
	if (!buf) return false;

	// Get clipboard id for HTML format...
	static int cfid = 0;
	if (!cfid) cfid = RegisterClipboardFormatA("HTML Format");

	// Create a template string for the HTML header...
	strcpy(buf,
		"Version:0.9\r\n"
		"StartHTML:00000000\r\n"
		"EndHTML:00000000\r\n"
		"StartFragment:00000000\r\n"
		"EndFragment:00000000\r\n"
		"<html><body>\r\n"
		"<!--StartFragment -->\r\n");

	// Append the HTML...
	strcat(buf, html.c_str());
	strcat(buf, "\r\n");
	// Finish up the HTML format...
	strcat(buf,
		"<!--EndFragment-->\r\n"
		"</body>\r\n"
		"</html>");

	// Now go back, calculate all the lengths, and write out the
	// necessary header information. Note, wsprintf() truncates the
	// string when you overwrite it so you follow up with code to replace
	// the 0 appended at the end with a '\r'...
	char *ptr = strstr(buf, "StartHTML");
	sprintf(ptr + 10, "%08u", strstr(buf, "<html>") - buf);
	*(ptr + 10 + 8) = '\r';

	ptr = strstr(buf, "EndHTML");
	sprintf(ptr + 8, "%08u", strlen(buf));
	*(ptr + 8 + 8) = '\r';

	ptr = strstr(buf, "StartFragment");
	sprintf(ptr + 14, "%08u", strstr(buf, "<!--StartFrag") - buf);
	*(ptr + 14 + 8) = '\r';

	ptr = strstr(buf, "EndFragment");
	sprintf(ptr + 12, "%08u", strstr(buf, "<!--EndFrag") - buf);
	*(ptr + 12 + 8) = '\r';

	// Open the clipboard...
	if (OpenClipboard(0)) {

		// Empty what's in there...
		EmptyClipboard();

		// Allocate global memory for transfer...
		HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strlen(buf) + 4);
		char *ptr = (char *)GlobalLock(hText);
		strcpy(ptr, buf);
		GlobalUnlock(hText);

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wplain = converter.from_bytes(plain);

		HGLOBAL hUnicodeText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, wcslen(wplain.c_str()) + 4);
		wchar_t *uptr = (wchar_t*)GlobalLock(hUnicodeText);
		wcscpy(uptr, wplain.c_str());
		GlobalUnlock(hUnicodeText);

		::SetClipboardData(cfid, hText);
		::SetClipboardData(CF_UNICODETEXT, hUnicodeText);

		CloseClipboard();

		// Free memory...
		GlobalFree(hText);
		GlobalFree(hUnicodeText);
	}

	// Clean up...
	delete[] buf;
	return true;
}

void PasteFromClipboard(FOREGROUND_ID id) {
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}