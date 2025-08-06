// Copyright 2025 Crystal Ferrai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <filesystem>
#include <set>
#include <Windows.h>

namespace BComp
{
	class Options;

	// Runs binary comparisons between files
	class Comparer
	{
	public:
		Comparer();
		~Comparer();

		int Compare(const Options& options);

	public:
		Comparer(const Comparer&) = delete;
		Comparer& operator=(const Comparer&) = delete;

	private:
		int CompareDirectories(const wchar_t* first, const wchar_t* second, const wchar_t* filter, bool recursive);
		int CompareFiles(const wchar_t* first, const wchar_t* second);

		bool GetFiles(const wchar_t* root, const wchar_t* directory, const wchar_t* filter, bool recursive, std::set<std::filesystem::path>& files);

		void LogWin32Error(const wchar_t* format, ...);
		void LogWin32Error(DWORD code, const wchar_t* format, ...);
		void LogWin32Error(DWORD code, const wchar_t* format, va_list args);

		void SetConsoleColor(WORD color);
		void RestoreConsoleColor();

	private:
		HANDLE mConsole;
		WORD mOriginalConsoleColor;
	};
}
