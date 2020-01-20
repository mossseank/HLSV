/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the general class members and file-level visit functions for the visitor.
 */

#include "visitor.hpp"


namespace hlsv
{

// ====================================================================================================================
Visitor::Visitor(const CompilerOptions* opt, std::unique_ptr<ReflectionInfo>* refl, antlr4::CommonTokenStream* ts) :
	options_{ opt },
	reflect_{ refl },
	tokens_{ ts }
{

}

// ====================================================================================================================
Visitor::~Visitor()
{

}

} // namespace hlsv
