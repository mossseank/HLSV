/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the Compiler class found in the public API.
 */

#include "hlsv-priv.hpp"


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

	return true;
}

} // namespace hlsv
