/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the Compiler class.

#include "config.hpp"
#include "error_listener.hpp"
#include "visitor.hpp"
#include "reflect/io.hpp"
#include "antlr/ANTLRInputStream.h"
#include "antlr/CommonTokenStream.h"
#include "../generated/HLSVLexer.h"
#include "../generated/HLSV.h"
#include <fstream>
#include <sstream>

#ifdef HLSV_COMPILER_MSVC
	// It never sees reflect_ change, so it complains about dereferencing a null pointer that isnt actually null
#	pragma warning( disable : 6011 )
#endif // HLSV_COMPILER_MSVC

#define SET_ERR(src,...) last_error_ = CompilerError(CompilerError::src, ##__VA_ARGS__);


namespace hlsv
{

// ====================================================================================================================
string CompilerError::get_rule_stack_str() const
{
	if (rule_stack.size() == 0)
		return "";

	std::stringstream ss{};
	const auto END = rule_stack.end();
	for (auto it = rule_stack.begin(); it != END; ++it) {
		ss << (*it);
		if ((it + 1) != END)
			ss << ", ";
	}
	return ss.str();
}

// ====================================================================================================================
CompilerOptions::CompilerOptions() :
	generate_reflection_file{ false },
	use_binary_reflection{ false }
{

}

// ====================================================================================================================
CompilerOptions::~CompilerOptions()
{

}

// ====================================================================================================================
Compiler::Compiler() :
	last_error_{ CompilerError::ES_NONE, "" },
	reflect_{ nullptr }
{

}

// ====================================================================================================================
Compiler::~Compiler()
{
	if (reflect_) {
		delete reflect_;
		reflect_ = nullptr;
	}
}

// ====================================================================================================================
bool Compiler::compile(const string& file, const CompilerOptions& options)
{
	// Read in the contents of the file
	std::ifstream infile{ file, std::ios::in };
	if (!infile.is_open()) {
		SET_ERR(ES_FILEIO, "Input file does not exist, or cannot be opened.");
		return false;
	}
	const string source = ([](std::ifstream& f) {
		std::stringstream ss;
		ss << f.rdbuf();
		return ss.str();
	})(infile);

	// Create the base ANTLR input objects
	antlr4::ANTLRInputStream inputStream{ source };
	grammar::HLSVLexer lexer{ &inputStream };
	antlr4::CommonTokenStream tokens{ &lexer };
	grammar::HLSV parser{ &tokens };

	// Register the custom listener
	ErrorListener listener{};
	lexer.removeErrorListeners();
	parser.removeErrorListeners();
	lexer.addErrorListener(&listener);
	parser.addErrorListener(&listener);

	// Perform the lexing and parsing, report any error
	auto fileCtx = parser.file();
	if (listener.has_error()) {
		last_error_ = listener.last_error;
		return false;
	}

	// Visit the tree (this is the generator step)
	Visitor visitor{ &tokens, &reflect_, &options };
	try
	{
		// Clear the potential previous reflection info before populating it again
		if (reflect_) {
			delete reflect_;
			reflect_ = nullptr;
		}
		auto any = visitor.visit(fileCtx);
	}
	catch (const VisitError& ve)
	{
		last_error_ = ve.error;
		// Clear the reflection info on error
		if (reflect_) {
			delete reflect_;
			reflect_ = nullptr;
		}
		return false;
	}

	// Generate the reflection info file
	if (options.generate_reflection_file) {
		auto writer = options.use_binary_reflection ? ReflWriter::WriteBinary : ReflWriter::WriteText;
		string err{};
		if (!writer("reflection.refl", *reflect_, err)) {
			SET_ERR(ES_FILEIO, strarg("Unable to write reflection file, reason: %s.", err.c_str()));
			return false;
		}
	}

	// All done and good to go (ensure the compiler error is cleared)
	SET_ERR(ES_NONE, "");
	return true;
}

} // namespace hlsv
