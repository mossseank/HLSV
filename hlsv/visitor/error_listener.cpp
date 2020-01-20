/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements error_listener.hpp
 */

#include "error_listener.hpp"
#include "../../generated/HLSV.h"


namespace hlsv
{

// ====================================================================================================================
ErrorListener::ErrorListener() :
	last_error{ nullptr }
{

}

// ====================================================================================================================
ErrorListener::~ErrorListener()
{
	last_error.reset();
}

// ====================================================================================================================
void ErrorListener::syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, 
	size_t charPositionInLine, const std::string& msg, std::exception_ptr e)
{
	namespace a4 = antlr4;

	// Get some error info
	a4::RuleContext* ctx = nullptr;
	string badText = offendingSymbol ? offendingSymbol->getText() : "";
	if (e) {
		try { std::rethrow_exception(e); }
		catch (const a4::RecognitionException& ex) {
			ctx = ex.getCtx();
			if (badText.empty() && ex.getOffendingToken()) {
				badText = ex.getOffendingToken()->getText();
			}
		}
	}
	int64 ridx = ctx ? (int64)ctx->getRuleIndex() : (int64)-1;

	////////////////////
	// !!TODO!! Expand this as we uncover more parsing and lexing errors
	////////////////////
	string errMsg = "";

	if (false) {

	}
	else { // Unknown Error
		errMsg = varstr("(Rule '%s') ~'%s'~ - %s",
			(ridx == -1) ? "none" : recognizer->getRuleNames()[ridx].c_str(),
			badText.c_str(), msg.c_str());
	}

	// Generate the error
	last_error.reset(new CompilerError(CompilerStage::Parse, errMsg));
	last_error->source = { badText, (uint32)line, (uint32)charPositionInLine };
}

} // namespace hlsv
