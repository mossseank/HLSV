/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the visitor, which is the object that performs the walk of the source tree.

#pragma once

#include "../config.hpp"
#include "../gen/glsl_generator.hpp"
#include "var_manager.hpp"
#include "../generated/HLSVBaseVisitor.h"
#include "expr.hpp"
#include "antlr/CommonTokenStream.h"

#define VISIT(vtype) antlrcpp::Any visit##vtype(grammar::HLSV::vtype##Context* ctx) override;

#define NEW_EXPR(name) auto name = new Expr;
#define NEW_EXPR_T(name, type) auto name = new Expr{(type)};
#define GET_VISIT_SPTR(vis) (std::shared_ptr<Expr>(visit(vis).as<Expr*>()))


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
	VisitError(CompilerError::error_source src, const string& msg, uint32 l, uint32 c, const string& bt) :
		std::runtime_error(msg), error{ src, msg, l, c, {} }
	{ 
		error.bad_text = bt;
	}
}; // class VisitError 

// The actual source tree visitor type
class Visitor final :
	public grammar::HLSVBaseVisitor
{
	friend class GLSLGenerator;

private:
	antlr4::CommonTokenStream* tokens_;
	ReflectionInfo** reflect_;
	const CompilerOptions* options_;
	GLSLGenerator gen_;
	VariableManager variables_;
	HLSVType infer_type_; // The type to use when inferring how to interpret an initializer list
	ShaderStages current_stage_;

public:
	Visitor(antlr4::CommonTokenStream* ts, ReflectionInfo** refl, const CompilerOptions* opt);
	~Visitor();

	inline void ERROR(antlr4::RuleContext* ctx, const string& msg) const {
		auto tk = tokens_->get(ctx->getSourceInterval().a);
		throw VisitError(CompilerError::ES_COMPILER, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), ctx->getText());
	}
	inline void ERROR(antlr4::Token* tk, const string& msg) const {
		throw VisitError(CompilerError::ES_COMPILER, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), tk->getText());
	}
	inline void ERROR(antlr4::tree::TerminalNode* node, const string& msg) const {
		auto tk = tokens_->get(node->getSourceInterval().a);
		throw VisitError(CompilerError::ES_COMPILER, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), node->getText());
	}

	inline GLSLGenerator& get_generator() { return gen_; }

	int64 parse_integer_literal(antlr4::Token* tk, bool* isuns, bool forceSize = false) const;
	int64 parse_integer_literal(antlr4::tree::TerminalNode* tn, bool* isuns, bool forceSize = false) const {
		return parse_integer_literal(tn->getSymbol(), isuns, forceSize);
	}
	inline uint32 parse_size_literal(antlr4::Token* tk) const {
		return (uint32)parse_integer_literal(tk, nullptr, true);
	}
	float parse_float_literal(antlr4::tree::TerminalNode* tk) const;
	Variable parse_variable(grammar::HLSV::VariableDeclarationContext* ctx, VarScope scope);

	// Core
	VISIT(File)
	VISIT(ShaderVersionStatement)
	VISIT(VertexAttributeStatement)
	VISIT(FragmentOutputStatement)
	VISIT(LocalStatement)
	VISIT(UniformStatement)
	VISIT(PushConstantsStatement)
	VISIT(ConstantStatement)
	VISIT(VertFunction)
	VISIT(FragFunction)

	// Statement
	VISIT(VariableDeclaration)
	VISIT(VariableDefinition)

	// Expr
	VISIT(PostfixExpr)
	VISIT(PrefixExpr)
	VISIT(FactorExpr)
	VISIT(NegateExpr)
	VISIT(MulDivModExpr)
	VISIT(AddSubExpr)
	VISIT(BitShiftExpr)
	VISIT(RelationalExpr)
	VISIT(EqualityExpr)
	VISIT(BitLogicExpr)
	VISIT(BoolLogicExpr)
	VISIT(TernaryExpr)
	VISIT(ArrayIndexerAtom)
	VISIT(SwizzleAtom)
	VISIT(InitializerList)
	VISIT(FunctionCall)
	VISIT(VariableAtom)
	VISIT(ScalarLiteral)

	Expr* visit_binary_expr(antlr4::RuleContext* ctx, antlr4::Token* op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);
}; // class Visitor

} // namespace hlsv


#undef VISIT
