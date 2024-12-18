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

#pragma once

#include "framework.h"

namespace winfx {

struct Point : public tagPOINT {
	Point(LONG x_,LONG y_) { x = x_; y = y_; }
};
	
struct Size : public tagSIZE {
	Size(LONG cx_,LONG cy_) { cx = cx_; cy = cy_; }
};

struct Rect : public tagRECT {
	Rect() { top = bottom = right = left = 0; }
	Rect(LONG l, LONG t, LONG r, LONG b) {
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	int height() { return bottom - top; }
	int width() { return right - left; }
	operator LPRECT () { return this; }
	operator LPCRECT () const { this; }
};

inline void DebugOut(LPCWSTR format ...) {
#ifdef _DEBUG
	wchar_t buffer[1024];
	va_list args;
	va_start(args, format);
	vswprintf_s(buffer, format, args);
	va_end(args);
	OutputDebugStringW(buffer);
#endif
}

class App {
public:
	App() {
		singleton_ = this;
	}

	virtual ~App();

	static App& getSingleton() { return *singleton_; }
		
	HINSTANCE getInstance() { return hInst_; }
	DWORD getExitCode() { return dwExitCode_; }
	std::wstring loadString(UINT uID) {
		wchar_t buffer[1024];
		::LoadStringW(hInst_, uID, buffer, 1024);
		return std::wstring(buffer);
	}

	virtual bool initInstance(HINSTANCE hInst, HINSTANCE hInstPrev);
	virtual bool initWindow(LPWSTR pwstrCmdLine, int nCmdShow) = 0;
	virtual bool translateModelessMessage(MSG* pmsg);

	// Runs the main message loop of the application
	void processMessages();

	// May be overridden by derived classes to perform cleanup before
	// exit.
	virtual void beforeTerminate();

	using HandlerFunction = std::function<HRESULT(void)>;
	HRESULT addEventHandler(HANDLE event, HandlerFunction handler);
	void removeEventHandler(HANDLE event);

protected:
	HINSTANCE hInst_ = 0;
	DWORD dwExitCode_  = 0;

	// Array of Handles to wait on in message loop, and a parallel array of
	// handlers to invoke when one of the handles is signaled.
	HANDLE wait_handles_[MAXIMUM_WAIT_OBJECTS] = { 0 };
	HandlerFunction handlers_[MAXIMUM_WAIT_OBJECTS] = { nullptr };
	DWORD handle_count_ = 0;

	static App* singleton_;
};

class Window {
protected:
	HWND			hwnd;
	Window*			pwndParent;
	std::wstring	className;
	std::wstring	windowName;
	bool			classIsRegistered;

public:
	
	explicit Window(Window* pwndParent_in = 0) :
		hwnd(0),
		pwndParent(pwndParent_in),
		classIsRegistered(false) {}

	Window(std::wstring classNameIn, Window* pwndParentIn = 0) :
		hwnd(0),
		pwndParent(pwndParentIn),
		className(classNameIn),
		classIsRegistered(false) {}

	Window(std::wstring classNameIn, std::wstring windowNameIn, Window* pwndParentIn = 0) :
		hwnd(0),
		pwndParent(pwndParentIn),
		className(classNameIn),
		windowName(windowNameIn),
		classIsRegistered(false) {}

	virtual ~Window();
	
	// Accessors
	HWND getWindow() { return hwnd; }

	// Creates the window as a top-level (overlapped) window
	virtual bool createAppWindow(LPWSTR pstrCmdLine, int nCmdShow);

	// Creates the window as a child window
	virtual bool createChildWindow(Point pos, Size size, DWORD child_window_id);

	void destroy() { ::DestroyWindow(hwnd); }

	HDC getDC() { return ::GetDC(hwnd); }
	int releaseDC(HDC hdc) { return ::ReleaseDC(hwnd, hdc); }

	Rect getClientRect() { Rect r; ::GetClientRect(hwnd, (LPRECT)r); return r; }
	Rect getWindowRect() { Rect r; ::GetWindowRect(hwnd, (LPRECT)r); return r; }
		
	int messageBox(const std::wstring& text, const std::wstring& caption, UINT uType = MB_OK ) {
		return ::MessageBox(hwnd, text.c_str(), caption.c_str(), uType);
	}
	int messageBox(const std::wstring& text, UINT uType = MB_OK ) {
		return messageBox(text, windowName, uType);
	}

	bool moveWindow(Point pos, Size size, bool repaint) {
		return (bool) ::MoveWindow(hwnd, pos.x, pos.y, size.cx, size.cy, repaint);
	}

	void showWindow(int nCmdShow) { ::ShowWindow(hwnd, nCmdShow); }
	bool setWindowPos(HWND hwndBefore, int x, int y, int cx, int cy, UINT uiFlags) {
		return ::SetWindowPos(hwnd, hwndBefore, x, y, cx, cy, uiFlags); 
	}

	void invalidateRect(BOOL repaint=TRUE) {
		::InvalidateRect(hwnd, NULL, repaint);
	}

	void invalidateRect(Rect rect, BOOL repaint=TRUE) {
		::InvalidateRect(hwnd, (LPRECT)rect, repaint);
	}

	bool postMessage(UINT msg, WPARAM wparam, LPARAM lparam) { 
		return (bool) ::PostMessage(hwnd, msg, wparam, lparam);
	}
		
	virtual LRESULT onClose(HWND hwnd);
	virtual LRESULT onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	
	bool registerWindowClass();
	virtual Point getDefaultWindowPosition();
	virtual Size getDefaultWindowSize();
		
	virtual void modifyWndClass(WNDCLASSEXW& wc);
	virtual LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Called by the WndProc to set the window handle during window creation.
	void setWindowHandle(HWND hwndParam) { hwnd = hwndParam; }
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Dialog : public Window {
protected:
	int idd;

	friend BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	Dialog(Window* pwnd, int idd_in) :
		Window(pwnd),
		idd(idd_in)
		{
		}

	HWND getDlgItem(int id) { return GetDlgItem( hwnd, id ); }
	LRESULT sendDlgItemMessage(int id, UINT msg, WPARAM wparam, LPARAM lparam) { 
		return SendDlgItemMessage(hwnd, id, msg, wparam, lparam); 
	}
		
	bool endDialog(int nResult);
		
	std::wstring getItemText(int id);
	void setItemText(int id, const std::wstring& str);
	
	int getItemInt(int id);
	void setItemInt(int id, int value);
	
	virtual void onOk() {}
	virtual void onCancel() {}

	virtual LRESULT onInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	virtual LRESULT onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)	;
	virtual LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void create();
	int doDialogBox();
};

inline bool Dialog::endDialog(int nResult) {
	return EndDialog(hwnd, nResult);
}

inline BOOL textOut(HDC hdc, int x, int y, const std::wstring& str) {
	return TextOutW(hdc, x, y, str.c_str(), static_cast<int>(str.size()));
}

inline std::wstring loadString(UINT uID) {
	return App::getSingleton().loadString(uID);
}

}  // namespace winfx

