/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the type that listens for errors emitted by the lexer and parser types

#include "error_listener.hpp"
#include "../generated/HLSV.h"

#define MSG_STR(stxt) (msg.find(stxt)!=string::npos)
#define IS_RULE(rtype) (ridx==grammar::HLSV::Rule##rtype)


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

	// Get some error info
	const RecognitionException* rex{ nullptr };
	RuleContext* ctx{ nullptr };
	std::string badText = offendingSymbol ? offendingSymbol->getText() : "";
	if (e) {
		try { std::rethrow_exception(e); }
		catch (const RecognitionException& ex) {
			ctx = ex.getCtx();
			if (badText.length() == 0 && ex.getOffendingToken())
				badText = ex.getOffendingToken()->getText();
		}
	}
	auto ridx = ctx ? (int64)ctx->getRuleIndex() : (int64)-1;

	// !!!! TODO: Expand this as we uncover more errors !!!!
	std::string errMsg = "";

	// Shader version statements
	if (MSG_STR("{'compute',"))
		errMsg = "Invalid shader type in shader version statement.";
	else if (MSG_STR("missing VERSION_LITERAL"))
		errMsg = "Invalid version in shader version statement.";
	else if (IS_RULE(ShaderVersionStatement))
		errMsg = "Invalid shader version statement.";
	else if (IS_RULE(ConstantStatement) && MSG_STR("expecting '='"))
		errMsg = "Must provide a value to constant statements.";
	else if (IS_RULE(Atom) && MSG_STR("expecting SWIZZLE"))
		errMsg = strarg("Cannot mix swizzle characters of different types (%s).", badText.c_str());
	// Unknown error
	else {
		errMsg = strarg("(Rule '%s') (Bad text: '%s') - %s", (ridx == -1) ? "none" : recognizer->getRuleNames()[ridx].c_str(),
			badText.c_str(), msg.c_str());
	}

	// Get the invocation stack, and generate the error
	auto stack = (recognizer && dynamic_cast<grammar::HLSV*>(recognizer)) ? dynamic_cast<grammar::HLSV*>(recognizer)->getRuleInvocationStack() :
		std::vector<std::string>{};
	std::reverse(stack.begin(), stack.end());
	last_error = CompilerError(CompilerError::ES_PARSER, errMsg, (uint32)line, (uint32)charPositionInLine, stack);
}

} // namespace hlsv
