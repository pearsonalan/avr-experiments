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

#include "Registry.h"

const RegistryKey RegistryKey::InvalidKey;
const RegistryKey RegistryKey::LocalMachine(HKEY_LOCAL_MACHINE);
const RegistryKey RegistryKey::CurrentUser(HKEY_CURRENT_USER);

RegistryKey::RegistryKey(const RegistryKey& parent_key, const std::wstring& path, REGSAM sam) {
	HKEY hkey = 0;
	status_ = RegOpenKeyExW(parent_key.key(), path.c_str(), 0, sam, &hkey);
	if (status_ == ERROR_SUCCESS) {
		winfx::DebugOut(L"Opened key %08x\n", hkey);
		hkey_ = hkey;
	}
}

RegistryKey::~RegistryKey() {
	if (hkey_ != 0 && hkey_ != HKEY_LOCAL_MACHINE && hkey_ != HKEY_CURRENT_USER) {
		winfx::DebugOut(L"Closing key %08x\n", hkey_);
		RegCloseKey(hkey_);
		hkey_ = 0;
		status_ = ERROR_INVALID_HANDLE;
	}
}


RegistryKey RegistryKey::openOrCreate(const std::wstring& key_name, REGSAM sam) const {
	HKEY hkey = 0;
	LSTATUS status;
	DWORD disposition = 0;

	status = RegCreateKeyExW(hkey_, key_name.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE,
							 sam, NULL, &hkey, &disposition);
	if (status == ERROR_SUCCESS) {
		winfx::DebugOut(L"Opened key %08x\n", hkey);
	} else {
		winfx::DebugOut(L"Error in openOrCreate: %d\n", status);
	}

	return RegistryKey(hkey, status);
}

LSTATUS RegistryKey::getStringValue(const std::wstring& value_name, std::wstring* result) const {
	if (status_ != ERROR_SUCCESS) {
		winfx::DebugOut(L"calling getStringValue on key with bad status %d\n", status_);
		return status_;
	}

	wchar_t data[1024] = { 0 };
	DWORD type, len = sizeof(data) - sizeof(wchar_t);
	LSTATUS status = RegQueryValueExW(hkey_, value_name.c_str(), NULL, &type, 
									  reinterpret_cast<LPBYTE>(data), &len);
	if (status != ERROR_SUCCESS) {
		winfx::DebugOut(L"Registry::getStringValue: Error in RegQueryValueExW: %d\n", status);
		return status;	
	}	

	if (type != REG_SZ) {
		winfx::DebugOut(L"Registry::getStringValue: Value is not a REG_SZ\n");
		return ERROR_DATATYPE_MISMATCH;
	}

	if (len < 0 || len >= sizeof(data)) {
		winfx::DebugOut(L"Registry::getStringValue: Invalid Size\n");
		return ERROR_INCORRECT_SIZE;
	}

	winfx::DebugOut(L"Data len is %d bytes\n", len);

	data[len/sizeof(wchar_t)] = L'0';
	*result = data;
	
	return ERROR_SUCCESS;
}

std::wstring RegistryKey::getStringValueOrEmptyString(const std::wstring& value_name) const {
	std::wstring result;
	getStringValue(value_name, &result);
	return result;
}

std::wstring RegistryKey::getStringValueOrDefault(const std::wstring& value_name,
		const std::wstring& default_value) const {
	std::wstring result;
	if (getStringValue(value_name, &result) != ERROR_SUCCESS) {
		return default_value;
	}
	return result;
}

LSTATUS RegistryKey::setStringValue(const std::wstring& value_name, const std::wstring& value) {
	if (status_ != ERROR_SUCCESS) {
		winfx::DebugOut(L"calling setStringValue on key with bad status %d\n", status_);
		return status_;
	}

	LSTATUS status = RegSetValueExW(hkey_, value_name.c_str(), NULL, REG_SZ, 
									reinterpret_cast<const BYTE*>(value.c_str()), 
									static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
	if (status != ERROR_SUCCESS) {
		winfx::DebugOut(L"Error setting reg value: %d\n", status);
	}
	return status;
}

LSTATUS RegistryKey::getIntegerValue(const std::wstring& value_name, DWORD* result) const {
	if (status_ != ERROR_SUCCESS) {
		winfx::DebugOut(L"calling getIntegerValue on key with bad status %d\n", status_);
		return status_;
	}

	DWORD type, len = sizeof(DWORD), value;
	LSTATUS status = RegQueryValueExW(hkey_, value_name.c_str(), NULL, &type, 
									  reinterpret_cast<LPBYTE>(&value), &len);
	if (status != ERROR_SUCCESS) {
		winfx::DebugOut(L"Registry::getIntegerValue: Error in RegQueryValueExW: %d\n", status);
		return status;	
	}	

	if (type != REG_DWORD) {
		winfx::DebugOut(L"Registry::getIntegerValue: Value is not a REG_DWORD\n");
		return ERROR_DATATYPE_MISMATCH;
	}

	if (len != sizeof(DWORD)) {
		winfx::DebugOut(L"Registry::getIntegerValue: Invalid Size\n");
		return ERROR_INCORRECT_SIZE;
	}

	*result = value;
	return ERROR_SUCCESS;
}

LSTATUS RegistryKey::setIntegerValue(const std::wstring& value_name, DWORD value) {
	if (status_ != ERROR_SUCCESS) {
		winfx::DebugOut(L"calling setIntegerValue on key with bad status %d\n", status_);
		return status_;
	}

	LSTATUS status = RegSetValueExW(hkey_, value_name.c_str(), NULL, REG_DWORD, 
									reinterpret_cast<const BYTE*>(&value), 
									sizeof(DWORD));
	if (status != ERROR_SUCCESS) {
		winfx::DebugOut(L"Error setting reg value: %d\n", status);
	}
	return status;
}
