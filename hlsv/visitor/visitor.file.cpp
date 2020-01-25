/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the general class members and file-level visit functions for the visitor.
 */

#include "visitor.hpp"

#ifdef HLSV_MSVC
	// Ignoring the return value of visit(...)
#	pragma warning( disable : 26444 )
#endif // HLSV_MSVC



namespace hlsv
{

// ====================================================================================================================
Visitor::Visitor(const CompilerOptions* opt, std::unique_ptr<ReflectionInfo>* refl, antlr4::CommonTokenStream* ts) :
	options_{ opt },
	reflect_{ refl },
	scopes_{ new ScopeTree },
	tokens_{ ts }
{

}

// ====================================================================================================================
Visitor::~Visitor()
{

}

// ====================================================================================================================
VISIT_DEF(File)
{
	// Shader statement must be first
	visit(ctx->shaderVersionStatement());

	return nullptr;
}

// ====================================================================================================================
VISIT_DEF(ShaderVersionStatement)
{
	// Extract version
	uint32 ver = std::atoi(ctx->VERSION_LITERAL()->getText().c_str());
	if (ver > HLSV_VERSION) {
		ERROR(ctx, varstr("Current tool version (%u) cannot compile shader version %u.", HLSV_VERSION, ver));
	}

	// Check type
	if (ctx->KW_COMPUTE()) {
		ERROR(ctx, "Compute shaders are not yet supported.");
	}

	// Create reflection info
	reflect_->reset(new ReflectionInfo(PipelineType::Graphics, HLSV_VERSION, ver));

	// Setup builtins
	scopes_->populateBuiltins(PipelineType::Graphics);

	return nullptr;
}

// ====================================================================================================================
VISIT_DEF(VertexAttributeStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(FragmentOutputStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(LocalStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(UniformStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(PushConstantsStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ConstantStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(VertFunction)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(FragFunction)
{
	return new Expr(SVType::PrimType::Void);
}

} // namespace hlsv
