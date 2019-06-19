/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the type that listens for errors emitted by the lexer and parser types

#pragma once

#include "config.hpp"
#include "antlr/BaseErrorListener.h"


namespace hlsv
{

// Error listener that is registered with the lexer and parser
class ErrorListener final :
	public antlr4::BaseErrorListener
{
public:
	CompilerError last_error;

public:
	ErrorListener();
	~ErrorListener() { }

	inline bool has_error() const { return last_error.source != CompilerError::ES_NONE; }

	void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine,
		const std::string& msg, std::exception_ptr e) override;
}; // class ErrorListener

} // namespace hlsv
