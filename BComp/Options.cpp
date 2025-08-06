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

#include "Options.h"

#include <iostream>

namespace BComp
{
	Options::Options()
		: mFirstPath(nullptr)
		, mSecondPath(nullptr)
		, mFileNameFilter(nullptr)
		, mRecurseDirectories(false)
	{
	}

	Options::~Options()
	{
	}

	Options::Options(const Options& other)
		: mFirstPath(other.mFirstPath)
		, mSecondPath(other.mSecondPath)
		, mFileNameFilter(other.mFileNameFilter)
		, mRecurseDirectories(other.mRecurseDirectories)
	{
	}

	Options& Options::operator=(const Options& other)
	{
		if (&other != this)
		{
			mFirstPath = other.mFirstPath;
			mSecondPath = other.mSecondPath;
			mFileNameFilter = other.mFileNameFilter;
			mRecurseDirectories = other.mRecurseDirectories;
		}
		return *this;
	}

	bool Options::ParseCommandLine(int argc, const wchar_t** argv)
	{
		if (argc == 1)
		{
			PrintUsage();
			return false;
		}

		if (argc < 3)
		{
			std::wcerr << L"[ERROR] Too few parameters. Run with no parameters to see usage.\n";
			return false;
		}

		const wchar_t* firstPath = nullptr;
		const wchar_t* secondPath = nullptr;
		const wchar_t* fileNameFilter = nullptr;
		bool recurseDirectories = false;

		int positionalIndex = 0;
		for (int i = 1; i < argc; ++i)
		{
			if (argv[i][0] == L'-')
			{
				switch (argv[i][1])
				{
				case L'f':
				case L'F':
					if (i == argc - 1)
					{
						std::wcerr << L"[ERROR] Missing argument for option'" << argv[i] << L"'. Run with no parameters to see usage.\n";
						return false;
					}
					++i;
					fileNameFilter = argv[i];
					break;
				case L'r':
				case L'R':
					recurseDirectories = true;
					break;
				default:
					std::wcerr << L"[ERROR] Unrecognized option '" << argv[i] << L"'. Run with no parameters to see usage.\n";
					return false;
				}
			}
			else
			{
				switch (positionalIndex)
				{
				case 0:
					firstPath = argv[i];
					break;
				case 1:
					secondPath = argv[i];
					break;
				default:
					std::wcerr << L"[ERROR] Too many positional parameters. Run with no parameters to see usage.\n";
					return false;
				}
				++positionalIndex;
			}
		}

		if (positionalIndex < 2)
		{
			std::wcerr << L"[ERROR] Too few positional parameters. Run with no parameters to see usage.\n";
			return false;
		}

		mFirstPath = firstPath;
		mSecondPath = secondPath;
		mFileNameFilter = fileNameFilter;
		mRecurseDirectories = recurseDirectories;

		return true;
	}

	void Options::PrintUsage()
	{
		std::wcout
			<< L"Performs a binary equality comparison on two files or two directories with\n"
			<< L"similar contents. Prints results to stdout and errors to stderr.\n"
			<< L"\n"
			<< L"Returns 0 if everything matches, 1 if there are any differences.\n"
			<< L"Returns -1 if any error occurs.\n"
			<< L"\n"
			<< L"Usage: bcomp [[-f Filter] [-r]] [First Path] [Second Path]\n"
			<< L"\n"
			<< L"  First Path   The first path to compare. Can be a file or directory.\n"
			<< L"\n"
			<< L"  Second Path  The second path to compare. Must match first path type (file.\n"
			<< L"               or directory)\n"
			<< L"\n"
			<< L"Options\n"
			<< L"\n"
			<< L"  -f Filter  If paths are directories, filter file names using this filter.\n"
			<< L"\n"
			<< L"  -r         If paths are directories, recursively compare all subdirectories.\n";
	}
}
