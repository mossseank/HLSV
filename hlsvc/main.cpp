/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file is the entry point for hlsvc, the reference command line HLSV compiler tool.

#include <hlsv/hlsv.hpp>
#include "console.hpp"
#include "args.hpp"
#include <sstream>


int main(int argc, char** argv)
{
	using namespace hlsv;

	// Parse the arguments, print help and exit if requested
	Args args;
	if (!Args::Parse(argc, argv, args)) {
		Console::Error(args.error);
		return -1;
	}
	if (args.help) {
		Args::PrintHelp();
		return 0;
	}

	// Compile the input files
	Compiler comp{};
	for (const auto& ifile : args.input_files) {
		Console::Infof("Compiling file %s.", ifile.c_str());
		Console::UseIndent(true);
		if (!comp.compile(ifile)) {
			// Report the error
			auto& err = comp.get_last_error();
			if (err.source == CompilerError::ES_FILEIO) {
				Console::Error(err.message);
			}
			else if (err.source == CompilerError::ES_PARSER) {
				Console::Errorf("[%u:%u] - %s", err.line, err.character, err.message.c_str());
				if (err.rule_stack.size() > 0) {
					Console::Errorf("Rule Stack: %s", err.get_rule_stack_str().c_str());
				}
			}
			else if (err.source == CompilerError::ES_COMPILER) {
				Console::Errorf("[%u:%u] - %s", err.line, err.character, err.message.c_str());
			}
			continue;
		}
		Console::UseIndent(false);
	}

	return 0;
}
