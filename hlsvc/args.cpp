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
#include <algorithm>


// ====================================================================================================================
static std::string parse_integer_arg(int argc, char** argv, int* idx, uint32_t* arg)
{
	if (*idx == (argc - 1))
		return "no argument specified";

	const std::string valstr = argv[*idx + 1];
	if (valstr[0] == '-') {
		if (valstr.length() > 1 && std::isdigit(valstr[1])) {
			(*idx) += 1;
			return "cannot specify negative integer";
		}
		else return "no argument specified";
	}
	else if (!std::isdigit(valstr[0])) return "no argument specified";
	if (std::find_if(valstr.begin(), valstr.end(), [](char c) { return !std::isdigit(c); }) != valstr.end())
		return "invalid integer";

	errno = 0;
	uint64_t val = std::strtoull(valstr.c_str(), nullptr, 10);
	(*idx) += 1;
	if (val >= UINT32_MAX || errno == ERANGE) {
		errno = 0;
		return "integer out of range";
	}

	*arg = (uint32_t)val;
	return "";
}

// ====================================================================================================================
Args::Args() :
	error{ "" },
	input_files{},
	help{ false },
	options{ }
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
			else if (flag == "r" || flag == "reflect") {
				args.options.generate_reflection_file = true;
			}
			else if (flag == "b" || flag == "binary") {
				args.options.use_binary_reflection = true;
				args.options.generate_reflection_file = true;
			}
			else if (flag == "i" || flag == "glsl") {
				args.options.keep_intermediate = true;
			}
			else if (flag.find("rl-") == 0) { // Resource limit flag
				const std::string rl = flag.substr(3);
				uint32_t rlval;
				auto err = parse_integer_arg(argc, argv, &ai, &rlval);
				if (err.size()) {
					Console::Warnf("Ignoring invalid resource limit argument: %s", err.c_str());
					continue;
				}

				if (rl == "attr")
					args.options.limits.vertex_attribute_slots = rlval;
				else if (rl == "frag")
					args.options.limits.fragment_outputs = rlval;
				else if (rl == "local")
					args.options.limits.local_slots = rlval;
				else {
					Console::Warnf("Unknown resource limit type '%s', ignoring.", rl.c_str());
					continue;
				}
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
		"  > -r;--reflect                        Generate a text file that contains shader reflection info.\n"
		"  > -b;--binary                         Use a binary format for the reflection file instead of text. This\n"
		"                                          flag will implicity activate the '--reflect' flag.\n"
		"  > -i;--glsl                           Generates the intermediate cross-compiled GLSL files.\n"
		"  > --rl-<type> ARG                     Sets the resource limit for the <type>, ARG must be a integer.\n"
		"                                          <type> must be one of:\n"
		"                                            attr - The number of vertex attribute slots (default 16)\n"
		"                                            frag - The number of fragment outputs (default 4)\n"
		"                                            local - The number of binding slots for locals (default 8)\n"
	);
}
