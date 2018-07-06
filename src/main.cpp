#include "base/command_line.h"
#include "base/base_paths.h"
#include "base/files/file_path.h"
#include "nativeui/nativeui.h"
#include "base/values.h"
#include "nativeui/util/function_caller.h"
#include "nativeui/events/keyboard_code_conversion.h"
#include <iostream>
#include <fstream>

#include "RegisterShortcut.h"
#include "HideFromTaskbar.h"
#include "ForegroundWindow.h"
#include "CaptureKeyboard.h"
#include "ClipboardHelper.h"

#if defined(OS_WIN)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  base::CommandLine::Init(0, nullptr);
#else
int main(int argc, const char *argv[]) {
  base::CommandLine::Init(argc, argv);
#endif

  // Intialize GUI toolkit.
  nu::Lifetime lifetime;

  // Initialize the global instance of nativeui.
  nu::State state;

  // Notification Tray Icon to indicate the app is running
  std::string trayTitle = "Hello World";
  base::FilePath::StringType str;
  str.assign(L"resources/MyIcon.ico");
  base::FilePath iconPath(str);
  scoped_refptr<nu::Tray> tray(new nu::Tray(new nu::Image(iconPath)));
  tray->SetTitle(trayTitle);

  // Add Exit Menu Item
  scoped_refptr<nu::Menu> menu(new nu::Menu());
  tray->SetMenu(menu.get());
  scoped_refptr<nu::MenuItem> menuItemExit(new nu::MenuItem(nu::MenuItem::Type::Label));
  menuItemExit->SetLabel("Exit");
  menuItemExit->on_click.Connect([](nu::MenuItem* self) {
	  nu::MessageLoop::Quit();
  });
  menu->Append(menuItemExit.get());
  
  // Load up the HTML for the selector
  std::string filename("resources/index.html");
  std::ifstream file(filename.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string html = buffer.str();

  nu::Browser* browser;
  nu::Window*  window;

  RegisterShortcut([&html, &browser, &window]() {
	  
	  auto activeWindow = ForegroundWindowGet();
	  Point point = ForegroundPositionGet(activeWindow);

	  nu::Browser::Options browserOptions;
	  browserOptions.context_menu = true;
	  browser = new nu::Browser(browserOptions);
	  browser->LoadHTML(html, "http://localhost/");

	  window = new nu::Window(nu::Window::Options());
	  window->SetContentView(browser);
	  window->SetContentSize(nu::SizeF(400, 400));
	  window->Center();
	  window->SetMaximizable(false);
	  window->SetMinimizable(false);
	  window->SetResizable(false);
	  window->SetAlwaysOnTop(true);
	  window->SetBounds(nu::RectF(point.x, point.y, 400, 400));

	  HideFromTaskbar(window);

	  window->Activate();

	  window->on_close.Connect([&browser, &window](nu::Window*) {
		browser->Release();
		window->Release();
		StopCapturingKeyboard();
	  });

	  ForegroundWindowSet(activeWindow);

	  browser->AddBinding("SendEmoji", [](nu::Browser* browser, const std::string& emoji, const std::string& fontSize) {
		  CopyToClipboard("<span style='font-size: " + fontSize + "'>" + emoji + "</span>", emoji);
		  auto id = ForegroundWindowGet();
		  browser->GetWindow()->Close(); // closure variables aren't allowed by the compiler
		  PasteFromClipboard(id);
	  });

	  StartCapturingKeyboard([&browser, &window](nu::KeyboardCode key) {
		  if (key == nu::KeyboardCode::VKEY_ESCAPE) {
			  window->Close();
		  }
		  else {
			  auto strKey = nu::KeyboardCodeToStr(key);
			  std::string code = "KeyEvent('" + std::string(strKey) + "')";
			  browser->ExecuteJavaScript(code, NULL);
		  }
	  });
  });
  
  // Enter message loop.
  nu::MessageLoop::Run();

  return 0;
}
