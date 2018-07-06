#include "ForegroundWindow.h"
#include "nativeui/nativeui.h"

FOREGROUND_ID ForegroundWindowGet() {
	return GetForegroundWindow();
}

void ForegroundWindowSet(FOREGROUND_ID id) {
	if (id > 0) {
		SetForegroundWindow(id);
	}
}

Point ForegroundPositionGet(FOREGROUND_ID id) {
	auto t1 = GetCurrentThreadId();
	auto t2 = GetWindowThreadProcessId(id, 0);
	AttachThreadInput(t1, t2, true);
	auto th = GetFocus();
	POINT point;
	GetCaretPos(&point);
	ClientToScreen(th, &point);
	AttachThreadInput(t1, t2, false);
	Point output;
	output.x = point.x;
	output.y = point.y;
	return output;
}