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
  std::string emojifilename("resources/index.html");
  std::ifstream emojifile(emojifilename.c_str());
  std::stringstream emojibuffer;
  emojibuffer << emojifile.rdbuf();
  std::string emojihtml = emojibuffer.str();

  // Load up the HTML for the selector
  std::string giffilename("resources/gif.html");
  std::ifstream giffile(giffilename.c_str());
  std::stringstream gifbuffer;
  gifbuffer << giffile.rdbuf();
  std::string gifhtml = gifbuffer.str();

  nu::Browser* browser;
  nu::Window*  window;

  RegisterShortcut([&emojihtml, &gifhtml, &browser, &window]() {
	  
	  auto activeWindow = ForegroundWindowGet();
	  Point point = ForegroundPositionGet(activeWindow);

	  nu::Browser::Options browserOptions;
	  browserOptions.context_menu = true;
	  browser = new nu::Browser(browserOptions);
	  browser->LoadHTML(emojihtml, "http://localhost/emoji.html");
	  
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

	  browser->AddBinding("SendGIF", [](nu::Browser* browser, const std::string& url) {
		  CopyToClipboard("<img src='" + url + "'>", "");
		  auto id = ForegroundWindowGet();
		  browser->GetWindow()->Close();
		  PasteFromClipboard(id);
	  });

	  StartCapturingKeyboard([&browser, &window, &emojihtml, &gifhtml](nu::KeyboardCode key) {
		  if (key == nu::KeyboardCode::VKEY_ESCAPE) {
			  window->Close();
		  } else if (key == 191) {
			  if (strcmp(browser->GetTitle().c_str(), "Emoji") == 0) {
				  browser->LoadHTML(gifhtml, "http://localhost/gif.html");
			  }
			  else {
				  browser->LoadHTML(emojihtml, "http://localhost/emoji.html");
			  }
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
