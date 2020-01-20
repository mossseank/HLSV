/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Type for reporting lexing and parsing errors.
 */

#pragma once

#include "../hlsv-priv.hpp"
#include <antlr/BaseErrorListener.h>


namespace hlsv
{

// Error listener for lexing and parsing
class ErrorListener final :
	public antlr4::BaseErrorListener
{
public:
	std::unique_ptr<CompilerError> last_error;

	ErrorListener();
	~ErrorListener();

	inline bool has_error() const { return !!last_error.get(); }

	void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine,
		const std::string& msg, std::exception_ptr e) override;
}; // class ErrorListener

} // namespace hlsv
