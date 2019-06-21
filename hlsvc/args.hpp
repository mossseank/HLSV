/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the functionality for parsing arguments from the command line

#pragma once

#include <hlsv/hlsv.hpp>
#include <string>
#include <vector>


class Args final
{
	using str = std::string;
	using strvec = std::vector<std::string>;

public:
	str error; // The error encountered while parsing the arguments (if Parse() returns false)
	strvec input_files; // The HLSV source files to compile (will have at least one if no error occurs)
	bool help;
	hlsv::CompilerOptions options;

public:
	Args();
	~Args() { }

	static bool Parse(int argc, char** argv, Args& args);

	static void PrintHelp();
}; // class Args
