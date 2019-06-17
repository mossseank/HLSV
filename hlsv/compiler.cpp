/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the Compiler class.

#include "config.hpp"
#include <fstream>
#include <sstream>


namespace hlsv
{

// ====================================================================================================================
Compiler::Compiler() :
	_last_error{ CompilerError::ES_NONE, "" }
{

}

// ====================================================================================================================
Compiler::~Compiler()
{

}

// ====================================================================================================================
bool Compiler::compile(const string& file)
{
	// Read in the contents of the file
	std::ifstream infile{ file, std::ios::in };
	if (!infile.is_open()) {
		_last_error = CompilerError(CompilerError::ES_FILEIO, "Input file does not exist, or cannot be opened.");
		return false;
	}
	const string source = ([](std::ifstream& f) {
		std::stringstream ss;
		ss << f.rdbuf();
		return ss.str();
	})(infile);

	// All done and good to go (ensure the compiler error is cleared)
	_last_error = CompilerError(CompilerError::ES_NONE, "");
	return true;
}

} // namespace hlsv
