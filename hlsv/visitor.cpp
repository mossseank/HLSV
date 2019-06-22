/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements visitor.hpp.

#include "visitor.hpp"
#include <cstdlib>

#ifdef HLSV_COMPILER_MSVC
	// Lots of warnings about ignoring the return value of visit...() functions
#	pragma warning( disable : 26444 )
#endif // HLSV_COMPILER_MSVC

#define VISIT_FUNC(vtype) antlrcpp::Any Visitor::visit##vtype(grammar::HLSV::vtype##Context* ctx)
#define REFL (*reflect_)


namespace hlsv
{

// ====================================================================================================================
Visitor::Visitor(antlr4::CommonTokenStream* ts, ReflectionInfo** refl, const CompilerOptions* opt) :
	tokens_{ ts },
	reflect_{ refl },
	options_{ opt },
	gen_{ }
{

}

// ====================================================================================================================
Visitor::~Visitor()
{

}

// ====================================================================================================================
uint32 Visitor::parse_size_literal(antlr4::Token* tk, uint32 limit)
{
	auto txt = tk->getText();
	uint64 val = std::strtoull(txt.c_str(), nullptr, 10);
	if (val >= limit) // Catches both out or range errors are parse errors
		ERROR(tk, strarg("Invalid or out of range integer value ('%s').", txt.c_str()));
	return (uint32)val;
}

// ====================================================================================================================
VISIT_FUNC(File)
{
	// Visit the version statement first
	visit(ctx->shaderVersionStatement());

	// Visit all of the top-level statements
	for (auto tls : ctx->topLevelStatement()) {
		visit(tls);
	}

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(ShaderVersionStatement)
{
	// Extract the version and check it
	uint32 ver = std::atoi(ctx->VERSION_LITERAL()->getText().c_str());
	if (ver > HLSV_VERSION)
		ERROR(ctx, strarg("Current tool version (%u) cannot compile requested shader version (%u).", HLSV_VERSION, ver));
	if (ctx->KW_COMPUTE())
		ERROR(ctx, strarg("Compute shaders are not supported by hlsvc version %u.", HLSV_VERSION));

	// Create and populate the initial reflection info
	*reflect_ = new ReflectionInfo{ ShaderType::Graphics, HLSV_VERSION, ver };

	// No return
	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(VertexAttributeStatement)
{
	// Extract the components
	auto index = parse_size_literal(ctx->Index);
	auto vdec = ctx->variableDeclaration();

	return nullptr;
}

} // namespace hlsv
