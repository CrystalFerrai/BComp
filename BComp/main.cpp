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

#include <conio.h>
#include <Windows.h>

#include "Comparer.h"
#include "Options.h"

#include <iostream>

using namespace BComp;

int wmain(int argc, const wchar_t** argv)
{
	Options options = Options();
	if (!options.ParseCommandLine(argc, argv))
	{
		return -1;
	}

	Comparer comparer = Comparer();
	int result = comparer.Compare(options);

	if (IsDebuggerPresent())
	{
#pragma warning(push)
#pragma warning(disable : 6031)
		_getch();
#pragma warning(pop)
	}

	return result;
}
