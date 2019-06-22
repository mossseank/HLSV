/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the visitor, which is the object that performs the walk of the source tree.

#pragma once

#include "config.hpp"
#include "gen/glsl_generator.hpp"
#include "../generated/HLSVBaseVisitor.h"
#include "antlr/CommonTokenStream.h"

#define VISIT(vtype) antlrcpp::Any visit##vtype(grammar::HLSV::vtype##Context* ctx) override;


namespace hlsv
{

// Used to propogate compiler errors for easier handling
class VisitError final :
	public std::runtime_error
{
public:
	CompilerError error;

public:
	VisitError(const CompilerError& e) :
		std::runtime_error(e.message), error{ e }
	{ }
	VisitError(CompilerError::error_source src, const string& msg, uint32 l = 0, uint32 c = 0, const std::vector<string> & rs = {}) :
		std::runtime_error(msg), error{ src, msg, l, c, rs }
	{ }
}; // class VisitError 

// The actual source tree visitor type
class Visitor final :
	public grammar::HLSVBaseVisitor
{
private:
	antlr4::CommonTokenStream* tokens_;
	ReflectionInfo** reflect_;
	const CompilerOptions* options_;
	GLSLGenerator gen_;

public:
	Visitor(antlr4::CommonTokenStream* ts, ReflectionInfo** refl, const CompilerOptions* opt);
	~Visitor();

	inline void ERROR(antlr4::RuleContext* ctx, const string& msg) {
		auto tk = tokens_->get(ctx->getSourceInterval().a);
		throw VisitError(CompilerError::ES_COMPILER, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine());
	}
	inline void ERROR(antlr4::Token* tk, const string& msg) {
		throw VisitError(CompilerError::ES_COMPILER, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine());
	}

	inline GLSLGenerator& get_generator() { return gen_; }

	uint32 parse_size_literal(antlr4::Token* tk, uint32 limit = UINT32_MAX);

	VISIT(File)
	VISIT(ShaderVersionStatement)
	VISIT(VertexAttributeStatement)
}; // class Visitor

} // namespace hlsv


#undef VISIT
