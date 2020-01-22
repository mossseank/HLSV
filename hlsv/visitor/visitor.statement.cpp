/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the statement-type syntax nodes in the Visitor.
 */

#include "visitor.hpp"

#ifdef HLSV_MSVC
 // Ignoring the return value of visit(...)
#	pragma warning( disable : 26444 )
#endif // HLSV_MSVC


namespace hlsv
{

// ====================================================================================================================
VISIT_DEF(VariableDeclaration)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(VariableDefinition)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(Assignment)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(Lvalue)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(IfStatement)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(WhileLoop)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(DoLoop)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ForLoop)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ForLoopUpdate)
{
	return new Expr(SVType::PrimType::Void);
}

// ====================================================================================================================
VISIT_DEF(ControlStatement)
{
	return new Expr(SVType::PrimType::Void);
}

} // namespace hlsv
