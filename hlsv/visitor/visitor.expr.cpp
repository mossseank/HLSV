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
#include <cmath>

#ifdef HLSV_COMPILER_MSVC
	// Lots of warnings about ignoring the return value of visit...() functions
#	pragma warning( disable : 26444 )
	// Incorrect "dereferencing null pointer" warnings
#	pragma warning( disable : 6011 )
#endif // HLSV_COMPILER_MSVC

#define VISIT_FUNC(vtype) antlrcpp::Any Visitor::visit##vtype(grammar::HLSV::vtype##Context* ctx)
#define REFL (*reflect_)
#define OPT (options_)
#define LIMITS (options_->limits)


namespace hlsv
{

// ====================================================================================================================
VISIT_FUNC(ArrayIndexerAtom)
{
	// Get the index
	auto idx = GET_VISIT_SPTR(ctx->Index);
	if (idx->type.is_array || !idx->type.is_integer_type() || !idx->type.is_scalar_type())
		ERROR(ctx->Index, "Arrays can only be accessed using scalar non-array integer types.");

	// Check the types to create the expression type properly
	auto val = GET_VISIT_SPTR(ctx->atom());
	HLSVType::PrimType etype = HLSVType::Error;
	if (val->type.is_array) {
		if (idx->is_literal && (idx->literal_value.ui >= val->type.count))
			ERROR(ctx->Index, strarg("The integer literal '%lld' is larger than the array it is accessing.", idx->literal_value.ui));
		etype = val->type.type;
	}
	else if (val->type.is_vector_type()) {
		if (idx->is_literal && (idx->literal_value.ui >= val->type.get_component_count()))
			ERROR(ctx->Index, strarg("The integer literal '%lld' is larger than the vector it is accessing.", idx->literal_value.ui));
		etype = val->type.get_component_type();
	}
	else if (val->type.is_matrix_type()) {
		auto size = (uint32)sqrt(val->type.get_component_count());
		auto ct = val->type.get_component_type();
		if (idx->is_literal && (idx->literal_value.ui >= size))
			ERROR(ctx->Index, strarg("The integer literal '%lld' is larger than the matrix it is accessing.", idx->literal_value.ui));
		etype = (HLSVType::PrimType)(ct + (size - 1)); // Get the correctly sized vector type
	}
	else
		ERROR(ctx->atom(), strarg("The type '%s' cannot have an array indexer applied.", val->type.get_type_str().c_str()));

	// Build the expression
	NEW_EXPR_T(expr, etype);
	expr->text = "(" + val->text + '[' + idx->text + "])";
	return expr;
}

// ====================================================================================================================
VISIT_FUNC(SwizzleAtom)
{
	// Get and validate swizzle
	auto stxt = ctx->SWIZZLE()->getText();
	if (stxt.length() > 4)
		ERROR(ctx->SWIZZLE(), "Swizzles cannot be larger than 4 components.");

	// Visit the value expression
	auto val = GET_VISIT_SPTR(ctx->atom());
	if (val->type.is_array || !val->type.is_vector_type())
		ERROR(ctx->atom(), "Can only apply swizzles to non-array vector types.");
	auto ct = val->type.get_component_type();
	auto cc = val->type.get_component_count();

	// Validate the components
	for (auto sc : stxt) {
		auto cidx = (sc == 'x' || sc == 'r' || sc == 's') ? 1u :
					(sc == 'y' || sc == 'g' || sc == 't') ? 2u :
					(sc == 'z' || sc == 'b' || sc == 'p') ? 3u :
					(sc == 'w' || sc == 'a' || sc == 'q') ? 4u : UINT32_MAX;
		if (cidx > cc) {
			ERROR(ctx->atom(), strarg("The type '%s' does not have the '%c' swizzle component.",
				val->type.get_type_str().c_str(), sc));
		}
	}

	// Build the expression
	auto nt = (HLSVType::PrimType)(ct + (stxt.length() - 1)); // Only works because of the ordering of the PrimType enum
	NEW_EXPR_T(expr, nt);
	expr->text = "(" + val->text + '.' + stxt + ')';
	return expr;
}

// ====================================================================================================================
VISIT_FUNC(InitializerList)
{
	if (infer_type_ == HLSVType::Error)
		ERROR(ctx, "Cannot infer type for initializer list from context.");

	if (infer_type_.is_array) {
		if (ctx->Args.size() >= 256u)
			ERROR(ctx, "Initializer lists cannot have more than 255 elements.");
		auto save_type = infer_type_;
		infer_type_ = infer_type_.type; // Keeps the type, but sets is_array to false to generate children

		// Visit the children and build init string
		std::stringstream ss{}; ss << (HLSVType::IsScalarType(infer_type_.type) ? "{ " : TypeHelper::GetGLSLStr(infer_type_.type) + "[]( ");
		bool first = true;
		bool cconst = true;
		for (auto c : ctx->Args) {
			auto aexpr = GET_VISIT_SPTR(c);
			if (aexpr->type.is_array)
				ERROR(c, "Initializer lists cannot contain arrays.");
			if (!TypeHelper::CanPromoteTo(aexpr->type.type, infer_type_.type)) {
				ERROR(c, strarg("Cannot promote type '%s' to array member type '%s'.", aexpr->type.get_type_str().c_str(),
					infer_type_.get_type_str().c_str()));
			}
			if (!first) ss << ", ";
			else first = false;
			ss << aexpr->text;
			cconst = cconst && aexpr->is_compile_constant;
		}
		ss << (HLSVType::IsScalarType(infer_type_.type) ? " }" : " )");

		// Return the expression
		NEW_EXPR(expr);
		expr->type = { infer_type_.type, (uint8)ctx->Args.size() };
		expr->is_compile_constant = cconst;
		expr->text = ss.str();
		infer_type_ = save_type;
		return expr;
	}
	else {
		if (HLSVType::IsScalarType(infer_type_.type))
			ERROR(ctx, "Initializer lists cannot be used on scalar types.");

		// Visit all of the arguments and build the init text
		auto save_type = infer_type_;
		std::vector<Expr*> args{};
		std::stringstream ss{}; ss << TypeHelper::GetGLSLStr(infer_type_.type) << "( ";
		infer_type_ = HLSVType::Error;
		bool cconst = true;
		for (auto a : ctx->Args) {
			auto aexpr = visit(a).as<Expr*>();
			if (args.size() != 0) ss << ", ";
			ss << aexpr->text;
			args.push_back(aexpr);
			cconst = cconst && aexpr->is_compile_constant;
		}
		ss << " )";
		infer_type_ = save_type;

		// Check the arguments
		string err{ "" };
		if (!FunctionRegistry::CheckConstructor(infer_type_.type, args, err))
			ERROR(ctx, err);

		// Return the expression
		for (auto arg : args) delete arg;
		NEW_EXPR_T(expr, infer_type_.type);
		expr->is_compile_constant = cconst;
		expr->text = ss.str();
		return expr;
	}
}

// ====================================================================================================================
VISIT_FUNC(FunctionCall)
{
	auto ctype = TypeHelper::ParseTypeStr(ctx->Name->getText());
	if (ctype != HLSVType::Error) { // Type construction or casting
		if (ctype == HLSVType::Void)
			ERROR(ctx, "Cannot construct 'void' type.");
		if (!HLSVType::IsValueType(ctype))
			ERROR(ctx, "Cannot construct non-value types.");

		// Visit all of the arguments and build the init text
		auto save_type = infer_type_;
		std::vector<Expr*> args{};
		std::stringstream ss{}; ss << TypeHelper::GetGLSLStr(ctype) << "( ";
		infer_type_ = HLSVType::Error;
		bool cconst = true;
		for (auto a : ctx->Args) {
			auto aexpr = visit(a).as<Expr*>();
			if (args.size() != 0) ss << ", ";
			ss << aexpr->text;
			args.push_back(aexpr);
			cconst = cconst && aexpr->is_compile_constant;
		}
		ss << " )";
		infer_type_ = save_type;

		// Check the arguments
		string err{ "" };
		if (!FunctionRegistry::CheckConstructor(ctype, args, err))
			ERROR(ctx, err);

		// Return the expression
		for (auto arg : args) delete arg;
		NEW_EXPR_T(expr, ctype);
		expr->is_compile_constant = cconst;
		expr->text = ss.str();
		return expr;
	}
	else { // Function call
		ERROR(ctx->Name, strarg("There is no function with the name '%s'.", ctx->Name->getText().c_str()));
		return nullptr;
	}
}

// ====================================================================================================================
VISIT_FUNC(VariableAtom)
{
	auto vrbl = variables_.find_variable(ctx->IDENTIFIER()->getText());
	if (!vrbl) {
		ERROR(ctx->IDENTIFIER(), strarg("A variable with the name '%s' does not exist in the current context.",
			ctx->IDENTIFIER()->getText().c_str()));
	}
	if (!(vrbl->can_read(current_stage_)))
		ERROR(ctx, strarg("The variable '%s' cannot be read in the current context.", ctx->IDENTIFIER()->getText().c_str()));
	NEW_EXPR_T(expr, vrbl->type);
	expr->is_compile_constant = vrbl->is_constant() || vrbl->is_push_constant();
	expr->text = Variable::GetOutputName(vrbl->name);
	return expr;
}

// ====================================================================================================================
VISIT_FUNC(ScalarLiteral)
{
	NEW_EXPR(expr);
	expr->is_compile_constant = true;
	expr->is_literal = true;

	if (ctx->BOOLEAN_LITERAL()) {
		expr->type = HLSVType::Bool;
		expr->set_literal_value(ctx->BOOLEAN_LITERAL()->getText() == "true");
	}
	else if (ctx->FLOAT_LITERAL()) {
		expr->type = HLSVType::Float;
		expr->set_literal_value(parse_float_literal(ctx->FLOAT_LITERAL()));
	}
	else { // int
		bool isuns;
		auto lval = parse_integer_literal(ctx->INTEGER_LITERAL(), &isuns);
		if (isuns) {
			expr->type = HLSVType::UInt;
			expr->set_literal_value((uint32)lval);
		}
		else {
			expr->type = HLSVType::Int;
			expr->set_literal_value((int32)lval);
		}
	}

	return expr;
}

} // namespace hlsv
