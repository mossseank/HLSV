/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements args.hpp

#include "args.hpp"
#include "console.hpp"


// ====================================================================================================================
Args::Args() :
	error{ "" },
	input_files{},
	help{ false }
{

}

// ====================================================================================================================
/* static */ 
bool Args::Parse(int argc, char** argv, Args& args)
{
	args = {};

	// Loop over the arguments
	for (int ai = 1; ai < argc; ++ai) {
		const std::string arg = argv[ai];

		if (arg[0] == '-') { // The argument is a flag
			// Extract the flag without the dash(es), ignore invalid ones
			if ((arg.length() == 1) || (arg.length() == 2 && arg[1] == '-'))
				continue;
			const std::string flag = arg.substr((arg[1] == '-') ? 2 : 1);

			// Check the arguments
			if (flag == "h" || flag == "help" || flag == "?") {
				args.help = true;
				return true; // Cut out early since this flag forces the program to only display the help text
			}
			else {
				Console::Warnf("Unknown flag: %s.", arg.c_str());
			}
		}
		else { // Not associated with a flag, so its an input file
			args.input_files.push_back(arg);
		}
	}

	// Make sure at least one input file was specified
	if (args.input_files.size() == 0) {
		args.error = "No input file specified, use '-h' to see the help text.";
		return false;
	}

	return true;
}

// ====================================================================================================================
/* static */
void Args::PrintHelp()
{
	Console::Info(
		"\n"
		"hlsvc\n"
		"-----\n"
		"hlsvc is the command line tool for compiling HLSV (High-Level SPIR-V) shader code into SPIR-V bytecode.\n"
		"Additionally, the tool can generate reflection information and intermediate GLSL source code. At least\n"
		"one input file is required, and it is expected to have the extension '.hlsv'. For each input file,\n"
		"multiple files can be generated, the bytecode file ending in '.spv', the reflection file ending in\n"
		"'.refl', and GLSL source files ending in their stage name. The bytecode and reflection files can also be\n"
		"combined into a '.spvr' file. The order of arguments and input files does not matter. The output files\n"
		"will default into the same directory as the input files.\n"
		"\n"
		"Flags/Options:\n"
		"  > -h;-?;--help                        Prints this help message, then exits.\n"
	);
}
