// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see < https://www.gnu.org/licenses/>.

#include "winfx.h"

namespace winfx {
	
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Window::~Window() {
}

bool Window::createAppWindow(LPWSTR pstrCmdLine, int nCmdShow) {
	if (!classIsRegistered) {
		if (!registerWindowClass()) {
			winfx::DebugOut(L"Failed to register window class: %08x\n", GetLastError());
			return false;
		}
	}

	Point pos = getDefaultWindowPosition();
	Size  sz  = getDefaultWindowSize();
		
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  className.c_str(), windowName.c_str(),
						  WS_OVERLAPPEDWINDOW,
						  pos.x, pos.y,
						  sz.cx, sz.cy,
						  HWND_DESKTOP, NULL,
						  App::getSingleton().getInstance(), this);
	if (!hwnd) {
		winfx::DebugOut(L"Could not create window\n");
		return false;
	}

	ShowWindow(hwnd, nCmdShow);

	return true;
}

bool Window::createChildWindow(Point pos, Size size, DWORD child_window_id) {
	if (!classIsRegistered) {
		if (!registerWindowClass()) {
			winfx::DebugOut(L"Failed to register window class: %08x\n", GetLastError());
			return false;
		}
	}

	hwnd = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,  // Defaults
						  className.c_str(), L"",
						  WS_CHILD,
						  pos.x, pos.y,
						  size.cx, size.cy,
						  pwndParent->getWindow(),
						  reinterpret_cast<HMENU>(static_cast<int64_t>(child_window_id)),
						  App::getSingleton().getInstance(), this);
	if (!hwnd) {
		winfx::DebugOut(L"Could not create window\n");
		return false;
	}
	ShowWindow(hwnd, SW_SHOW);
	return false;
}

Point Window::getDefaultWindowPosition() {
	return Point(CW_USEDEFAULT, CW_USEDEFAULT);
}

Size Window::getDefaultWindowSize() {
	return Size(CW_USEDEFAULT, CW_USEDEFAULT);
}
	
LRESULT Window::onClose(HWND hwndParam) {
	PostQuitMessage(0);
	return 0;
}

LRESULT Window::onCreate(HWND hwndParam, LPCREATESTRUCT lpCreateStruct) {
	return 1;
}

bool Window::registerWindowClass() {
	WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
	 
	// 
	// Register window classes 
	// 

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc; 
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = sizeof(void*);
	wc.hInstance	 = App::getSingleton().getInstance();
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1); 
	wc.lpszMenuName  = 0; 
	wc.lpszClassName = className.c_str(); 

	modifyWndClass(wc);

	// don't allow modification of these
	wc.lpfnWndProc   = WndProc; 
	wc.cbWndExtra	 = sizeof(void*);

	if (!wc.lpszClassName)
		return false;
	
	if (!::RegisterClassEx(&wc)) 
		return false; 

	classIsRegistered = true;
	return true;
}

void Window::modifyWndClass(WNDCLASSEXW& wc) { 
}

LRESULT Window::handleWindowMessage(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_CREATE, onCreate);
		HANDLE_MSG(hwndParam, WM_CLOSE, onClose);
	}
	
	return DefWindowProc( hwndParam, uMsg, wParam, lParam );
}

LRESULT CALLBACK WndProc(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window* pWnd;

	if (uMsg == WM_CREATE) {
		pWnd = (Window*) (((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwndParam, GWLP_USERDATA, (LONG_PTR)pWnd);
		pWnd->setWindowHandle(hwndParam);
	} else
		pWnd = (Window*) GetWindowLongPtr(hwndParam, GWLP_USERDATA);

	if (!pWnd)
		return DefWindowProc(hwndParam, uMsg, wParam, lParam );

	return pWnd->handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

std::wstring Dialog::getItemText(int id) {
	HWND hwndCtl = ::GetDlgItem(hwnd, id);
	if (hwndCtl) {
		int len = Edit_GetTextLength(hwndCtl) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(_malloca(len * sizeof(wchar_t)));
		if (buffer) {
			Edit_GetText(hwndCtl, buffer, len);
			return std::wstring(buffer);
		}
	}
	return std::wstring();
}

void Dialog::setItemText(int id, const std::wstring& str) {
	HWND hwndCtl = GetDlgItem(hwnd, id );
	if (hwndCtl) {
		::SetWindowText(hwndCtl, str.c_str());
	}
}

int Dialog::getItemInt(int id) {
	return ::GetDlgItemInt(hwnd, id, NULL, TRUE);
}

void Dialog::setItemInt(int id, int value) {
	::SetDlgItemInt(hwnd, id, (UINT)value, TRUE);
}

LRESULT Dialog::onInitDialog(HWND hwndParam, HWND hwndFocus, LPARAM lParam) {
	return TRUE;
}

LRESULT Dialog::onCommand(HWND hwndParam, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id) {
	case IDOK:
		onOk();
		endDialog(IDOK);
		break;

	case IDCANCEL:
		onCancel();
		endDialog(IDCANCEL);
		break;
	}

	return TRUE;
}

LRESULT Dialog::handleWindowMessage(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_INITDIALOG, onInitDialog);
		HANDLE_MSG(hwndParam, WM_COMMAND, onCommand);
	}

	return FALSE;
}

void Dialog::create() {
	hwnd = CreateDialogParam(App::getSingleton().getInstance(), MAKEINTRESOURCE(idd),
							 pwndParent->getWindow(), (DLGPROC)DialogProc, (LPARAM)this);
}

int Dialog::doDialogBox() {
	return (int) DialogBoxParam(App::getSingleton().getInstance(), MAKEINTRESOURCE(idd),
						  pwndParent->getWindow(), (DLGPROC)DialogProc, (LPARAM)this);
}

BOOL CALLBACK DialogProc(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Dialog* pdlg;

	if (uMsg == WM_INITDIALOG) {
		pdlg = (Dialog*) lParam ;
		SetWindowLongPtr( hwndParam, DWLP_USER, lParam );
		pdlg->hwnd = hwndParam;
	} else {
		pdlg = (Dialog*) GetWindowLongPtr( hwndParam, DWLP_USER );
	}

	if (!pdlg)
		return FALSE;

	return (BOOL) pdlg->handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

App::~App() {
}

void App::beforeTerminate() {
}

bool App::initInstance( HINSTANCE hInstParam, HINSTANCE hInstPrev ) {
	hInst_ = hInstParam;
	return true;
}

bool App::translateModelessMessage(MSG* pmsg) {
	return false;
}

void App::processMessages() {
	for (;;) {
		// winfx::DebugOut(L"Waiting for an event or message");
		DWORD wait_result =
			::MsgWaitForMultipleObjectsEx(handle_count_,
										  wait_handles_,
										  INFINITE,
										  QS_ALLEVENTS, 
										  MWMO_INPUTAVAILABLE);
		if (wait_result >= WAIT_OBJECT_0 &&
			wait_result < WAIT_OBJECT_0 + handle_count_) {
			// One of the handles is signaled
			int signaled_event_index = wait_result - WAIT_OBJECT_0;
			// winfx::DebugOut(L"Event %d is signaled", signaled_event_index);
			HandlerFunction handler = handlers_[signaled_event_index];
			handler();
		} else if (wait_result == WAIT_OBJECT_0 + handle_count_) {
			// A windows message is available for processing.
			// winfx::DebugOut(L"Input events are available");
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				// winfx::DebugOut(L"Peeked message %d", msg.message);
				if (!translateModelessMessage(&msg)) {
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				if (msg.message == WM_QUIT) {
					winfx::DebugOut(L"Exiting on quit message");
					return;
				}
			}
			// winfx::DebugOut(L"No more input events");
		} else if (wait_result == WAIT_TIMEOUT) {
			// Unexpected if we aren't using a timer...
		} else if (wait_result == WAIT_FAILED) {
			// Unexpected
			winfx::DebugOut(L"Wait failure");
			return;
		} else {
			// Shouldn't be any other result.
			winfx::DebugOut(L"Unexpected Wait result %d", wait_result);
			return;
		}
	}
}

HRESULT App::addEventHandler(HANDLE event, App::HandlerFunction handler) {
	if (handle_count_ == MAXIMUM_WAIT_OBJECTS) {
		return E_FAIL;
	}

	wait_handles_[handle_count_] = event;
	handlers_[handle_count_] = handler;
	handle_count_++;
	winfx::DebugOut(L"Added handler [%08X] to app. New handle count = %d\n", (intptr_t)event, handle_count_);

	return S_OK;
}

void App::removeEventHandler(HANDLE event) {
	// Iterate through the array to find the handle to remove
	for (int i = 0; i < handle_count_; i++) {
		if (wait_handles_[i] == event) {
			// move all of the handles after the i-th handle down one
			for (int j = i; j + 1 < handle_count_; j++) {
				wait_handles_[j] = wait_handles_[j+1];
				handlers_[j] = handlers_[j+1];
			}
			wait_handles_[handle_count_ - 1] = 0;
			handlers_[handle_count_ - 1] = nullptr;
			handle_count_--;
			winfx::DebugOut(L"Removed handler [%08X] from app. New handle count = %d\n", (intptr_t)event, handle_count_);
			break;
		}
	}
}

App* App::singleton_ = NULL;

}  // namespace winfx

int APIENTRY wWinMain(_In_ HINSTANCE     hInst,
					  _In_opt_ HINSTANCE hInstPrev,
					  _In_ LPWSTR        pwstrCmdLine,
					  _In_ int           nCmdShow) {
	winfx::App& app = winfx::App::getSingleton();
	
	if (!app.initInstance(hInst,hInstPrev))
		return app.getExitCode();

	if (!app.initWindow(pwstrCmdLine, nCmdShow))
		return app.getExitCode();
	app.processMessages();
	app.beforeTerminate();

	return app.getExitCode();
}
