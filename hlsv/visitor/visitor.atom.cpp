/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the functions of the visitor class that deal with atomic expressions

#include "visitor.hpp"
#include "../type/functions.hpp"
#include <sstream>

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
VISIT_FUNC(ConstValue)
{
	if (ctx->scalarLiteral())
		return visit(ctx->scalarLiteral());
	else if (ctx->TypeName) { // Type constructor
		auto ctype = TypeHelper::ParseTypeStr(ctx->TypeName->getText());
		if (ctype == HLSVType::Error)
			ERROR(ctx->TypeName, strarg("Type constructor '%s' not a valid type.", ctx->TypeName->getText().c_str()));
		if (!HLSVType::IsValueType(ctype))
			ERROR(ctx->TypeName, "Constant expressions must use value types only.");
		if (HLSVType::IsScalarType(ctype))
			ERROR(ctx->TypeName, "Scalar casting is not allowed in constant expressions.");

		// Visit all of the arguments and build the init text
		auto save_type = infer_type_;
		infer_type_ = HLSVType::Error;
		std::vector<Expr*> args{};
		std::stringstream ss{}; ss << TypeHelper::GetGLSLStr(ctype) << "( ";
		for (auto a : ctx->Args) {
			auto aexpr = visit(a).as<Expr*>();
			if (args.size() != 0) ss << ", ";
			ss << aexpr->init_text;
			args.push_back(aexpr);
		}
		ss << " )";
		infer_type_ = save_type;

		// Check the arguments
		string err{ "" };
		if (!FunctionRegistry::CheckConstructor(ctype, args, err))
			ERROR(ctx->Args[0], err);

		// Return the expression
		NEW_EXPR_T(expr, ctype);
		expr->is_compile_constant = true;
		expr->is_literal = true;
		expr->init_text = ss.str();
		return expr;
	}
	else { // initializer list
		if (infer_type_.is_array) {
			if (ctx->constInitializerList()->Args.size() >= 256u)
				ERROR(ctx->constInitializerList(), "Initializer lists cannot have more than 255 elements.");
			auto save_type = infer_type_;
			infer_type_ = infer_type_.type; // Keeps the type, but sets is_array to false to generate children

			// Visit the children and build init string
			std::stringstream ss{}; ss << (HLSVType::IsScalarType(infer_type_.type) ? "{ " : TypeHelper::GetGLSLStr(infer_type_.type) + "[]( ");
			bool first = true;
			for (auto c : ctx->constInitializerList()->Args) {
				auto aexpr = GET_VISIT_SPTR(c);
				if (!TypeHelper::CanPromoteTo(aexpr->type.type, infer_type_.type)) {
					ERROR(c, strarg("Cannot promote type '%s' to array member type '%s'.", aexpr->type.get_type_str().c_str(),
						infer_type_.get_type_str().c_str()));
				}
				if (!first) ss << ", ";
				else first = false;
				ss << aexpr->init_text;
			}
			ss << (HLSVType::IsScalarType(infer_type_.type) ? " }" : " )");

			// Return the expression
			NEW_EXPR(expr);
			expr->type = { infer_type_.type, (uint8)ctx->constInitializerList()->Args.size() };
			expr->is_compile_constant = true;
			expr->init_text = ss.str();
			infer_type_ = save_type;
			return expr;
		}
		else {
			if (infer_type_.type == HLSVType::Error)
				ERROR(ctx, "Unable to infer type for initializer list.");
			if (HLSVType::IsScalarType(infer_type_.type))
				ERROR(ctx, "Initializer lists cannot be used on scalar types.");

			// Visit all of the arguments and build the init text
			std::vector<Expr*> args{};
			std::stringstream ss{}; ss << TypeHelper::GetGLSLStr(infer_type_.type) << "( ";
			for (auto a : ctx->constInitializerList()->Args) {
				auto aexpr = visit(a).as<Expr*>();
				if (args.size() != 0) ss << ", ";
				ss << aexpr->init_text;
				args.push_back(aexpr);
			}
			ss << " )";

			// Check the arguments
			string err{ "" };
			if (!FunctionRegistry::CheckConstructor(infer_type_.type, args, err))
				ERROR(ctx->constInitializerList(), err);

			// Return the expression
			NEW_EXPR_T(expr, infer_type_.type);
			expr->init_text = ss.str();
			return expr;
		}
	}
}

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
