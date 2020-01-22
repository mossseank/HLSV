/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the expression-type syntax nodes in the Visitor.
 */

#include "visitor.hpp"

#ifdef HLSV_MSVC
 // Ignoring the return value of visit(...)
#	pragma warning( disable : 26444 )
#endif // HLSV_MSVC


namespace hlsv
{

// ====================================================================================================================
VISIT_DEF(PostfixExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(PrefixExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(FactorExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(NegateExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(MulDivModExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(AddSubExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(BitShiftExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(RelationalExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(EqualityExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(BitLogicExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(BoolLogicExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(TernaryExpr)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ParenAtom)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ArrayIndexerAtom)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(SwizzleAtom)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(InitializerList)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(FunctionCall)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(VariableAtom)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ScalarLiteral)
{
	return new Expr(SVType::PrimType::Void);
}

} // namespace hlsv
