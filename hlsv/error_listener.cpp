/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the type that listens for errors emitted by the lexer and parser types

#include "error_listener.hpp"


namespace hlsv
{

// ====================================================================================================================
ErrorListener::ErrorListener() :
	last_error{ CompilerError::ES_NONE, "" }
{

}

// ====================================================================================================================
void ErrorListener::syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine,
	const std::string& msg, std::exception_ptr e)
{
	using namespace antlr4;
}

} // namespace hlsv
