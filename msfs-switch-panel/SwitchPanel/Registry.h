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

#include "winfx.h"

class RegistryKey {
public:
	// Constructs an invalid key.
	RegistryKey() {}

	RegistryKey(HKEY hkey, LSTATUS status) : hkey_(hkey), status_(status) {}

	// Constructs a wrapper key around the given HKEY.
	explicit RegistryKey(HKEY hkey) : RegistryKey(hkey, ERROR_SUCCESS) {}

	// Opens the registry key named "path" under the parent key.
	RegistryKey(const RegistryKey& parent_key, const std::wstring& path, REGSAM sam = KEY_ALL_ACCESS);

	~RegistryKey();

	HKEY key() const { return hkey_; }
	LSTATUS status() const { return status_; }

	bool operator==(const RegistryKey& key) const { return hkey_ == key.hkey_; }
	bool operator!=(const RegistryKey& key) const { return hkey_ != key.hkey_; }

	// Opens or creates a sub key under this key with the given name
	RegistryKey openOrCreate(const std::wstring& key_name, REGSAM sam = KEY_ALL_ACCESS) const;

	// Retrieves a string value (REG_SZ) from this key with the given value name and returns
	// the result in the out parameter result. If an error occurs or a data type mismatch,
	// and error status is returned and the out parameter is not changed.
	LSTATUS getStringValue(const std::wstring& value_name, std::wstring* result) const;

	// Returns the string value (REG_SZ) of the given value or returns an empty string if an
	// error or data type mismatch occurs.
	std::wstring getStringValueOrEmptyString(const std::wstring& value_name) const;

	// Returns the string value (REG_SZ) of the given value or returns the given default
	// string if an error or data type mismatch occurs.
	std::wstring getStringValueOrDefault(const std::wstring& value_name,
		const std::wstring& default_value) const;

	// Sets the given string value (REG_SZ).
	LSTATUS setStringValue(const std::wstring& value_name, const std::wstring& value);


	LSTATUS getIntegerValue(const std::wstring& value_name, DWORD* result) const;

	DWORD getIntegerValueOrDefault(const std::wstring& value_name, DWORD default_value) const {
		DWORD value = default_value;
		getIntegerValue(value_name, &value);
		return value;
	}

	DWORD getIntegerValueOrZero(const std::wstring& value_name) const {
		return getIntegerValueOrDefault(value_name, 0);
	}

	LSTATUS setIntegerValue(const std::wstring& value_name, DWORD value);

	static const RegistryKey InvalidKey;
	static const RegistryKey LocalMachine;
	static const RegistryKey CurrentUser;

private:
	HKEY hkey_ = 0;
	LSTATUS status_ = ERROR_INVALID_HANDLE;
};