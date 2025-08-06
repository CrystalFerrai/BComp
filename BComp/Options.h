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

namespace BComp
{
	// Program options derived from command line arguments
	class Options
	{
	public:
		inline const wchar_t* FirstPath() const { return mFirstPath; }
		inline const wchar_t* SecondPath() const { return mSecondPath; }
		inline const wchar_t* FileNameFilter() const { return mFileNameFilter; }
		inline bool RecurseDirectories() const { return mRecurseDirectories; }

	public:
		bool ParseCommandLine(int argc, const wchar_t** argv);
		static void PrintUsage();

	public:
		Options();
		Options(const Options& other);
		Options& operator=(const Options& other);
		~Options();

	private:
		const wchar_t* mFirstPath;
		const wchar_t* mSecondPath;
		const wchar_t* mFileNameFilter;
		bool mRecurseDirectories;
	};
}
