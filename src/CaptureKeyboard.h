#pragma once

#include <functional>
#include "nativeui/events/keyboard_code_conversion.h"

void StartCapturingKeyboard(std::function<void(nu::KeyboardCode)>);
void StopCapturingKeyboard();
