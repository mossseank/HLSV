/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Declares the primary source tree visitor type for Antlr.
 */

#pragma once

#include "../hlsv-priv.hpp"
#include "expr.hpp"
#include "scope/scope_tree.hpp"
#include <stdexcept>

#include "../pre-antlr.hpp"
#include "../../generated/HLSVBaseVisitor.h"
#include <antlr/CommonTokenStream.h>
#include "../post-antlr.hpp"

#define VISIT_DECL(vtype) antlrcpp::Any visit##vtype(grammar::HLSV::vtype##Context* ctx) override;


namespace hlsv
{

// Exception used to propogate compiler errors
class VisitError final :
	public std::runtime_error
{
public:
	CompilerError error;

	VisitError(const CompilerError& e) :
		std::runtime_error(e.message), error{ e }
	{ }
	VisitError(CompilerStage stage, const string& msg, uint32 l, uint32 c, const string& bad) :
		std::runtime_error(msg), error{ stage, msg }
	{
		error.source = { bad, l, c };
	}
}; // class VisitError

// Main ANTLR visitor type
class Visitor final :
	public grammar::HLSVBaseVisitor
{
private:
	const CompilerOptions* options_;
	std::unique_ptr<ReflectionInfo>* reflect_;
	std::unique_ptr<ScopeTree> scopes_;
	antlr4::CommonTokenStream* tokens_;

public:
	Visitor(const CompilerOptions* opt, std::unique_ptr<ReflectionInfo>* refl, antlr4::CommonTokenStream* ts);
	~Visitor();

	inline void ERROR(antlr4::RuleContext* ctx, const string& msg) const {
		auto tk = tokens_->get(ctx->getSourceInterval().a);
		throw VisitError(CompilerStage::Parse, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), ctx->getText());
	}
	inline void ERROR(antlr4::Token* tk, const string& msg) const {
		throw VisitError(CompilerStage::Parse, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), tk->getText());
	}
	inline void ERROR(antlr4::tree::TerminalNode* node, const string& msg) const {
		auto tk = tokens_->get(node->getSourceInterval().a);
		throw VisitError(CompilerStage::Parse, msg, (uint32)tk->getLine(), (uint32)tk->getCharPositionInLine(), node->getText());
	}

	// File-Level
	VISIT_DECL(File)
	VISIT_DECL(ShaderVersionStatement)
	VISIT_DECL(VertexAttributeStatement)
	VISIT_DECL(FragmentOutputStatement)
	VISIT_DECL(LocalStatement)
	VISIT_DECL(UniformStatement)
	VISIT_DECL(PushConstantsStatement)
	VISIT_DECL(ConstantStatement)
	VISIT_DECL(VertFunction)
	VISIT_DECL(FragFunction)

	// Statements
	VISIT_DECL(VariableDeclaration)
	VISIT_DECL(VariableDefinition)
	VISIT_DECL(Assignment)
	VISIT_DECL(Lvalue)
	VISIT_DECL(IfStatement)
	VISIT_DECL(WhileLoop)
	VISIT_DECL(DoLoop)
	VISIT_DECL(ForLoop)
	VISIT_DECL(ForLoopUpdate)
	VISIT_DECL(ControlStatement)

	// Expressions
	VISIT_DECL(PostfixExpr)
	VISIT_DECL(PrefixExpr)
	VISIT_DECL(FactorExpr)
	VISIT_DECL(NegateExpr)
	VISIT_DECL(MulDivModExpr)
	VISIT_DECL(AddSubExpr)
	VISIT_DECL(BitShiftExpr)
	VISIT_DECL(RelationalExpr)
	VISIT_DECL(EqualityExpr)
	VISIT_DECL(BitLogicExpr)
	VISIT_DECL(BoolLogicExpr)
	VISIT_DECL(TernaryExpr)
	VISIT_DECL(ParenAtom)
	VISIT_DECL(ArrayIndexerAtom)
	VISIT_DECL(SwizzleAtom)
	VISIT_DECL(InitializerList)
	VISIT_DECL(FunctionCall)
	VISIT_DECL(VariableAtom)
	VISIT_DECL(ScalarLiteral)
};

} // namespace hlsv


#undef VISIT_DECL

#define VISIT_DEF(vtype) antlrcpp::Any Visitor::visit##vtype(grammar::HLSV::vtype##Context* ctx)
#define NEW_EXPR(name, type) auto name = new Expr{(type)};
#define VISIT(vis) (std::shared_ptr<Expr>(visit(vis).as<Expr*>()))
#define VREFL (*reflect_)
#define VOPT (options_)
#define VLIMS (options_->limits)
#define VFEATS (options_->features)
