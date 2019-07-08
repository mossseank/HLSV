/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the functions of the visitor class that deal with atomic expressions

#include "visitor.hpp"

#ifdef HLSV_COMPILER_MSVC
	// Lots of warnings about ignoring the return value of visit...() functions
#	pragma warning( disable : 26444 )
#endif // HLSV_COMPILER_MSVC

#define VISIT_FUNC(vtype) antlrcpp::Any Visitor::visit##vtype(grammar::HLSV::vtype##Context* ctx)
#define REFL (*reflect_)
#define OPT (options_)
#define LIMITS (options_->limits)


namespace hlsv
{

// ====================================================================================================================
VISIT_FUNC(ScalarLiteral)
{
	NEW_EXPR(expr);
	expr->is_compile_constant = true;
	expr->is_literal = true;

	if (ctx->BOOLEAN_LITERAL()) {
		expr->type = HLSVType::Bool;
		expr->set_default_value(ctx->BOOLEAN_LITERAL()->getText() == "true");
	}
	else if (ctx->FLOAT_LITERAL()) {
		expr->type = HLSVType::Float;
		expr->set_default_value(parse_float_literal(ctx->FLOAT_LITERAL()));
	}
	else { // int
		bool isuns;
		expr->set_default_value(parse_integer_literal(ctx->INTEGER_LITERAL(), &isuns));
		expr->type = isuns ? HLSVType::UInt : HLSVType::Int;
	}

	return expr;
}

} // namespace hlsv
