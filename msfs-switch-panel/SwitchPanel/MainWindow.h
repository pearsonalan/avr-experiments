// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see < https://www.gnu.org/licenses/>.

#pragma once

#include "framework.h"
#include "winfx.h"
#include "SimInterface.h"
#include "Serial.h"
#include "common/Protocol.h"
#include "Resource.h"

#define ID_TIMER_SIM_CONNECT 100
#define ID_TIMER_POLL_SIM    101

class MainWindow : public winfx::Window, public SimulatorCallbacks, public SerialNotificationSink {
public:
	MainWindow() : 
		winfx::Window(winfx::loadString(IDC_SWITCHPANELEX), winfx::loadString(IDS_APP_TITLE)) {
		sim_.addCallback(this);
	}

	void modifyWndClass(WNDCLASSEXW& wc) override;
	bool createAppWindow(LPWSTR pstrCmdLine, int nCmdShow) override;

	LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	winfx::Size getDefaultWindowSize() override {
		return winfx::Size(400, 450);
	}

	LRESULT onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;

	void onSimDataUpdated(const SimData* data) override;
	void onStateChange(SimulatorInterfaceState state) override;
	void onSimDisconnect() override;
	
	// Callbacks for SerialNotificationSink
	void onReceivedData(const BYTE* data, int len) override;
	void onDisconnected() override;

protected:
	BOOL AddNotificationIcon();
	BOOL DeleteNotificationIcon();
	void ShowContextMenu(HWND hwnd, winfx::Point point);

	HRESULT connectSim();
	LRESULT onActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
	void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void onDestroy(HWND hwnd);
	void onPaint(HWND hwnd);
	void onTimer(HWND hwnd, UINT idTimer);
	void onNotifyCallback(HWND, UINT idNotify, winfx::Point point);

	void connectSerial();
	void disconnectSerial();

	void handleProtocolMessage(ProtocolMessage message);

private:
	SimulatorInterface sim_;
	Serial serial_;
	Protocol protocol_;
	bool in_stat_message_ = false;

	bool taxi_lights_on_ = false;
	bool landing_lights_on_ = false;

	bool left_gear_down_ = false;
	bool right_gear_down_ = false;
	bool center_gear_down_ = false;
};

class AboutDialog : public winfx::Dialog {
public:
	AboutDialog(Window* pwnd) : winfx::Dialog(pwnd, IDD_ABOUTBOX) {}
};
