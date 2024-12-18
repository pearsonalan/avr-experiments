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

#include "PreferencesDialog.h"
#include "Registry.h"
#include "Preferences.h"

LRESULT PreferencesDialog::onInitDialog(HWND hwndParam, HWND hwndFocus, LPARAM lParam) {
	winfx::DebugOut(L"PreferencesDialog::onInitDialog\n");
	RegistryKey settings_key = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	if (settings_key != RegistryKey::InvalidKey) {
		std::wstring com_port = settings_key.getStringValueOrDefault(
			kPortRegistryValueName, kDefaultComPort);
		int baud_rate = settings_key.getIntegerValueOrDefault(
			kBaudRateRegistryValueName, kDefaultBaudRate);
		winfx::DebugOut(L"COM Port is %s, baud rate id %d\n",
			com_port.c_str(), baud_rate);
		setItemText(IDC_COM_PORT, com_port);
		setItemInt(IDC_BAUD_RATE, baud_rate);
	}
	return TRUE;
}

void PreferencesDialog::onOk() {
	std::wstring com_port = getItemText(IDC_COM_PORT);
	int baud_rate = getItemInt(IDC_BAUD_RATE);
	winfx::DebugOut(L"On OK. ComPort = %s, baud rate = %d\n", com_port.c_str(), baud_rate);
	RegistryKey settings_key = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	settings_key.setStringValue(L"Port", com_port);
	settings_key.setIntegerValue(L"BaudRate", baud_rate);
}