/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file is the entry point for hlsvc, the reference command line HLSV compiler tool.

#include <hlsv/hlsv.hpp>
#include <iostream>


int main(int argc, char** argv)
{
	// Clip the command, check the number of args present
	if (argc < 2) {
		std::cout << "ERROR: input file argument required." << std::endl;
		return -1;
	}

	// Compile the input file
	hlsv::Compiler comp{};
	if (!comp.compile(argv[1])) {
		std::cout << "ERROR: compilation failed." << std::endl;
		return -1;
	}

	return 0;
}
