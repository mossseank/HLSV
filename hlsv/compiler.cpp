/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the Compiler class found in the public API.
 */

#include "hlsv-priv.hpp"
#include "visitor/error_listener.hpp"
#include "visitor/visitor.hpp"
#include <antlr/ANTLRInputStream.h>
#include <antlr/CommonTokenStream.h>
#include "../generated/HLSV.h"
#include "../generated/HLSVLexer.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;
namespace a4 = antlr4;

#ifdef HLSV_MSVC
	// It never sees reflect_ change, so it complains about dereferencing a null pointer that isnt actually null
#	pragma warning( disable : 6011 )
#endif // HLSV_MSVC

#define SET_ERR(stage,...) last_error_.reset(new CompilerError(CompilerStage::stage, ##__VA_ARGS__));


namespace hlsv
{

// ====================================================================================================================
Compiler::Compiler() :
	last_error_{ nullptr },
	reflection_{ nullptr },
	paths_{ }
{

}

// ====================================================================================================================
Compiler::~Compiler()
{
	last_error_.reset();
	reflection_.reset();
}

// ====================================================================================================================
bool Compiler::compile_file(const string& path, const CompilerOptions& options)
{
	// Clear previous state
	last_error_.reset();
	reflection_.reset();

	// Setup filepaths
	if (!prepare_paths(path, options)) return false;

	// Read in the contents of the sourcefile
	const string source = ([](const string& path) -> string {
		std::ifstream file{ path, std::ios::in };
		if (!file.is_open()) {
			return "<ERROR>";
		}
		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
	})(paths_.input_path);
	if (source == "<ERROR>") {
		SET_ERR(FileRead, "Input file could not be opened.");
		return false;
	}

	// Base ANTLR parse objects
	a4::ANTLRInputStream inputStream{ source };
	grammar::HLSVLexer lexer{ &inputStream };
	a4::CommonTokenStream tokens{ &lexer };
	grammar::HLSV parser{ &tokens };

	// Custom listener registration
	ErrorListener listener{};
	lexer.removeErrorListeners();
	parser.removeErrorListeners();
	lexer.addErrorListener(&listener);
	parser.addErrorListener(&listener);

	// Perform lexing and parsing
	auto fileCtx = parser.file();
	if (listener.has_error()) {
		last_error_ = std::move(listener.last_error);
		return false;
	}

	// Visit the source tree
	reflection_.reset();
	Visitor visitor{ &options, &reflection_, &tokens };
	try {
		auto res = visitor.visit(fileCtx);
	}
	catch (const VisitError& ve) {
		last_error_.reset(new CompilerError{ ve.error });
		reflection_.reset();
		return false;
	}

	// No errors
	last_error_.reset();
	return true;
}

// ====================================================================================================================
bool Compiler::prepare_paths(const string& path, const CompilerOptions& options)
{
	paths_ = {};
	std::error_code fileErr{};

	// Validate input path
	fs::path inPath{ path };
	if (auto type{ fs::status(inPath).type() };
		(type == fs::file_type::none) || (type == fs::file_type::not_found) || 
		(type == fs::file_type::directory)) 
	{
		return ERROR(CompilerStage::FileRead, 
			varstr("Input path '%s' is invalid or does not exist.", path.c_str()), path);
	}
	inPath = fs::absolute(inPath).lexically_normal();
	paths_.input_path = inPath.string();
	paths_.input_file = inPath.filename().string();

	// Generate the output path
	if (!options.output_path.empty()) {
		fs::path spvFile{ fs::absolute({ options.output_path }, fileErr).lexically_normal() };
		if (auto type{ fs::status(spvFile).type() };
			(type == fs::file_type::none) || (type == fs::file_type::directory)) 
		{
			return ERROR(CompilerStage::FileRead,
				varstr("Output path override '%s' is not valid.", options.output_path.c_str()), options.output_path);
		}
		paths_.output_path = spvFile.string();
	}
	else {
		paths_.output_path = inPath.replace_extension(".spv").string();
	}

	// Generate the reflection path
	if (!options.reflection_path.empty()) {
		fs::path rflFile{ fs::absolute({ options.reflection_path }, fileErr).lexically_normal() };
		if (auto type{ fs::status(rflFile).type() };
			(type == fs::file_type::none) || (type == fs::file_type::directory)) 
		{
			return ERROR(CompilerStage::FileRead,
				varstr("Reflection path override '%s' is not valid.", options.reflection_path.c_str()), options.reflection_path);
		}
		paths_.reflection_path = rflFile.string();
	}
	else {
		paths_.reflection_path = fs::path(paths_.output_path).replace_extension(".rfl").string();
	}

	return true;
}

// ====================================================================================================================
bool Compiler::ERROR(CompilerStage stage, const string& msg, const string& path)
{
	last_error_.reset(new CompilerError(stage, msg));
	last_error_->fileio.file_name = path;
	return false;
}

// ====================================================================================================================
bool Compiler::ERROR(CompilerStage stage, const string& msg, const string& bad, uint32 line, uint32 cpos)
{
	last_error_.reset(new CompilerError(stage, msg));
	last_error_->source = { bad, line, cpos };
	return false;
}

} // namespace hlsv
