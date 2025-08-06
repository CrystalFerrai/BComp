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

#include "Comparer.h"

#include <iostream>
#include <Shlwapi.h>

#include "Options.h"
#include "Utils.h"

extern "C" int CompareFiles(const wchar_t* aPath, const wchar_t* bPath);

namespace BComp
{
	Comparer::Comparer()
		: mConsole(INVALID_HANDLE_VALUE)
		, mOriginalConsoleColor(0)
	{
		mConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (mConsole != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO bufferInfo = {};
			if (GetConsoleScreenBufferInfo(mConsole, &bufferInfo))
			{
				mOriginalConsoleColor = bufferInfo.wAttributes;
			}
			else
			{
				mConsole = INVALID_HANDLE_VALUE;
			}
		}
	}

	Comparer::~Comparer()
	{
	}

	int Comparer::Compare(const Options& options)
	{
		wchar_t firstPath[MAX_PATH];
		wchar_t secondPath[MAX_PATH];

		wcscpy_s(firstPath, MAX_PATH, options.FirstPath());
		wcscpy_s(secondPath, MAX_PATH, options.SecondPath());

		size_t firstLen = wcslen(firstPath);
		size_t secondLen = wcslen(secondPath);

		// Remove trailing '\' character if present
		if (firstPath[firstLen - 1] == L'\\')
		{
			firstPath[firstLen - 1] = 0;
			--firstLen;
		}
		if (secondPath[secondLen - 1] == L'\\')
		{
			secondPath[secondLen - 1] = 0;
			--secondLen;
		}

		bool firstIsDirectory = PathIsDirectoryW(firstPath);
		bool secondIsDirectory = PathIsDirectoryW(secondPath);

		if (firstIsDirectory != secondIsDirectory)
		{
			std::wcerr << L"[ERROR] Passed in paths must be either both directories or both files.";
			return -1;
		}

		if (firstIsDirectory)
		{
			std::wcout
				<< L"Comparing directories:" << std::endl
				<< firstPath << std::endl
				<< secondPath << std::endl
				<< std::endl;

			return CompareDirectories(firstPath, secondPath, options.FileNameFilter(), options.RecurseDirectories());
		}

		return CompareFiles(firstPath, secondPath);
	}

	int Comparer::CompareDirectories(const wchar_t* first, const wchar_t* second, const wchar_t* filter, bool recursive)
	{
		std::set<std::filesystem::path> firstFiles;
		if (!GetFiles(first, L"", filter, recursive, firstFiles))
		{
			return -1;
		}

		std::set<std::filesystem::path> secondFiles;
		if (!GetFiles(second, L"", filter, recursive, secondFiles))
		{
			return -1;
		}

		int result = 0;

		std::filesystem::path firstRoot = first;
		std::filesystem::path secondRoot = second;

		for (const std::filesystem::path& path : firstFiles)
		{
			auto secondFile = secondFiles.find(path);
			if (secondFile == secondFiles.end())
			{
				SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
				std::wcout << L"[MISSING]  Second directory does not contain: " << path << std::endl;
				RestoreConsoleColor();

				result |= 1;
				continue;
			}
			else
			{
				secondFiles.erase(secondFile);
			}

			std::filesystem::path firstPath = firstRoot / path;
			std::filesystem::path secondPath = secondRoot / path;

			int cmp = ::CompareFiles(firstPath.c_str(), secondPath.c_str());
			switch (cmp)
			{
			case 0:
				SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
				std::wcout << L"[MISMATCH] " << path << std::endl;
				RestoreConsoleColor();
				result |= 1;
				break;
			case 1:
				SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				std::wcout << L"[MATCH]    " << path << std::endl;
				RestoreConsoleColor();
				break;
			default:
				SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);
				std::wcerr << L"[ERROR]    Failed to compare " << path << std::endl;
				RestoreConsoleColor();
				result = -1;
				break;
			}
		}

		for (const std::filesystem::path& path : secondFiles)
		{
			SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
			std::wcout << L"[MISSING]  First directory does not contain: " << path << std::endl;
			RestoreConsoleColor();

			result |= 1;
		}

		switch (result)
		{
		case 0:
			std::wcout << L"\nAll files match\n";
			break;
		case 1:
			std::wcout << L"\nSome mismatched files were found\n";
			break;
		default:
			std::wcout << L"\nSome errors occurred when comparing files\n";
			break;
		}

		return result;
	}

	int Comparer::CompareFiles(const wchar_t* first, const wchar_t* second)
	{
		int result = ::CompareFiles(first, second);
		switch (result)
		{
		case 0:
			SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
			std::wcout << L"[MISMATCH] " << first << L" != " << second << std::endl;
			RestoreConsoleColor();
			return 1;
		case 1:
			SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			std::wcout << L"[MATCH]    " << first << L" == " << second << std::endl;
			RestoreConsoleColor();
			return 0;
		default:
			SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);
			std::wcerr << L"[ERROR]    Failed to compare " << first << L", " << second << std::endl;
			RestoreConsoleColor();
			return -1;
		}
	}

	bool Comparer::GetFiles(const wchar_t* root, const wchar_t* directory, const wchar_t* filter, bool recursive, std::set<std::filesystem::path>& files)
	{
		// Locate matching files
		{
			std::filesystem::path searchPath = std::filesystem::path(root) / directory / (filter ? filter : L"*");

			WIN32_FIND_DATAW findData = {};
			HANDLE searchHandle = FindFirstFileExW(searchPath.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
			if (searchHandle == INVALID_HANDLE_VALUE)
			{
				DWORD code = GetLastError();
				if (code != ERROR_FILE_NOT_FOUND)
				{
					LogWin32Error(code, L"[ERROR] Failed to search directory '%s'.", directory);
					return false;
				}
			}
			else
			{
				bool continueSearch = true;
				while (continueSearch)
				{
					files.insert(std::filesystem::path(directory) / findData.cFileName);

					if (!FindNextFileW(searchHandle, &findData))
					{
						DWORD code = GetLastError();
						if (code != ERROR_NO_MORE_FILES)
						{
							LogWin32Error(code, L"[ERROR]    Error reading files in '%s'", directory);
						}
						FindClose(searchHandle);
						continueSearch = false;
					}
				}
			}
		}

		bool result = true;

		// Recurse into subdirectories
		if (recursive)
		{
			std::filesystem::path searchPath = std::filesystem::path(root) / directory / L"*";

			WIN32_FIND_DATAW findData = {};
			HANDLE searchHandle = FindFirstFileExW(searchPath.c_str(), FindExInfoBasic, &findData, FindExSearchLimitToDirectories, NULL, FIND_FIRST_EX_LARGE_FETCH);
			if (searchHandle == INVALID_HANDLE_VALUE)
			{
				DWORD code = GetLastError();
				if (code != ERROR_FILE_NOT_FOUND)
				{
					LogWin32Error(code, L"[ERROR] Failed to search directory '%s'.", directory);
					return false;
				}
			}
			else
			{
				bool continueSearch = true;
				while (continueSearch)
				{
					if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
					{
						std::filesystem::path subdir = std::filesystem::path(directory) / findData.cFileName;
						result &= GetFiles(root, subdir.c_str(), filter, recursive, files);
					}

					if (!FindNextFileW(searchHandle, &findData))
					{
						DWORD code = GetLastError();
						if (code != ERROR_NO_MORE_FILES)
						{
							LogWin32Error(code, L"[ERROR]    Error reading '%s'", directory);
						}
						FindClose(searchHandle);
						continueSearch = false;
					}
				}
			}
		}

		return result;
	}

	void Comparer::LogWin32Error(const wchar_t* format, ...)
	{
		DWORD code = GetLastError();

		va_list args;
		va_start(args, format);
		LogWin32Error(code, format, args);
		va_end(args);
	}

	void Comparer::LogWin32Error(DWORD code, const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		LogWin32Error(code, format, args);
		va_end(args);
	}

	void Comparer::LogWin32Error(DWORD code, const wchar_t* format, va_list args)
	{
		SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);

		vfwprintf(stderr, format, args);
		fwprintf(stderr, L" ");

		PrintError(code);

		RestoreConsoleColor();
	}

	void Comparer::SetConsoleColor(WORD color)
	{
		if (mConsole != INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(mConsole, color);
		}
	}

	void Comparer::RestoreConsoleColor()
	{
		if (mConsole != INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(mConsole, mOriginalConsoleColor);
		}
	}
}
