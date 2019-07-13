/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the functions of the visitor class that are statements within functions

#include "visitor.hpp"

#define VISIT_FUNC(vtype) antlrcpp::Any Visitor::visit##vtype(grammar::HLSV::vtype##Context* ctx)
#define REFL (*reflect_)
#define OPT (options_)
#define LIMITS (options_->limits)


namespace hlsv
{

// ====================================================================================================================
VISIT_FUNC(VariableDeclaration)
{
	// Create and check variable
	auto vrbl = parse_variable(ctx, VarScope::Block);
	if (vrbl.type.is_array || !vrbl.type.is_value_type())
		ERROR(ctx->Type, "Function locals can only be non-array value types.");

	// Add and emit
	variables_.add_variable(vrbl);
	gen_.emit_variable_declaration(vrbl, nullptr);

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(VariableDefinition)
{
	// Create and check variable
	auto vrbl = parse_variable(ctx->variableDeclaration(), VarScope::Block);
	if (vrbl.type.is_array || !vrbl.type.is_value_type())
		ERROR(ctx->variableDeclaration()->Type, "Function locals can only be non-array value types.");

	// Visit the value
	infer_type_ = vrbl.type;
	auto expr = GET_VISIT_SPTR(ctx->Value);
	infer_type_ = HLSVType::Error;
	if (!TypeHelper::CanPromoteTo(expr->type.type, vrbl.type.type)) {
		ERROR(ctx->Value, strarg("The rvalue type '%s' cannot be promoted to type '%s'.", expr->type.get_type_str().c_str(),
			vrbl.type.get_type_str().c_str()));
	}

	// Add and emit
	variables_.add_variable(vrbl);
	gen_.emit_variable_declaration(vrbl, expr.get());

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(SimpleAssign)
{
	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(ComplexAssign)
{
	return nullptr;
}

} // namespace hlsv
