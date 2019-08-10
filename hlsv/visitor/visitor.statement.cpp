/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the functions of the visitor class that are statements within functions

#include "visitor.hpp"
#include <cmath>

#ifdef HLSV_COMPILER_MSVC
	// Incorrect "dereferencing null pointer" warnings
#	pragma warning( disable : 6011 )
	// Complaining about not using the return value of 'visit(...)'
#	pragma warning( disable : 26444 )
#endif // HLSV_COMPILER_MSVC

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
VISIT_FUNC(Assignment)
{
	// Get the variable
	auto lval = GET_VISIT_SPTR(ctx->LVal);

	// Visit the value
	infer_type_ = lval->type;
	auto expr = GET_VISIT_SPTR(ctx->Value);
	infer_type_ = HLSVType::Error;
	if (expr->type.is_array)
		ERROR(ctx->Value, "The value of an assignment cannot be an array.");
	if (ctx->Op->getText() == "=") { // Simple Assignment 
		if (!TypeHelper::CanPromoteTo(expr->type.type, lval->type.type)) {
			ERROR(ctx->Value, strarg("The value type '%s' cannot be promoted to the variable type '%s'.",
				expr->type.get_type_str().c_str(), lval->type.get_type_str().c_str()));
		}
	}
	else { // Complex Assignment
		string err{};
		HLSVType rtype{};
		if (!TypeHelper::CheckBinaryOperator(ctx->Op, lval->type, expr->type, rtype, err))
			ERROR(ctx, err);
		if (rtype != lval->type)
			ERROR(ctx->Value, "The result of the operation does not match the variable type.");
	}

	// Write the assignment
	gen_.emit_assignment(lval->text, ctx->Op->getText(), *expr.get());

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(Lvalue)
{
	if (ctx->Name) { // Simply a variable
		auto vname = ctx->Name->getText();
		auto vrbl = variables_.find_variable(vname);
		if (!vrbl)
			ERROR(ctx->Name, strarg("The variable '%s' does not exist in the current context.", vname.c_str()));
		if (!vrbl->can_write(current_stage_))
			ERROR(ctx->Name, strarg("The variable '%s' cannot be modified in the current context.", vname.c_str()));

		// Send the variable upwards unmodified
		NEW_EXPR_T(expr, vrbl->type);
		expr->text = Variable::GetOutputName(vrbl->name);
		return expr;
	}
	else if (ctx->SWIZZLE()) { // Swizzle
		// Get the nested lvalue and check
		auto lval = GET_VISIT_SPTR(ctx->LVal);
		if (lval->type.is_array)
			ERROR(ctx->SWIZZLE(), "Cannot apply swizzle to array type.");
		if (!lval->type.is_vector_type())
			ERROR(ctx->SWIZZLE(), "Cannot apply swizzle to non-vector type.");
		auto ct = lval->type.get_component_type();
		auto cc = lval->type.get_component_count();

		// Validate the components
		auto stxt = ctx->SWIZZLE()->getText();
		for (auto sc : stxt) {
			auto cidx = (sc == 'x' || sc == 'r' || sc == 's') ? 1u :
				(sc == 'y' || sc == 'g' || sc == 't') ? 2u :
				(sc == 'z' || sc == 'b' || sc == 'p') ? 3u :
				(sc == 'w' || sc == 'a' || sc == 'q') ? 4u : UINT32_MAX;
			if (cidx > cc) {
				ERROR(ctx->SWIZZLE(), strarg("The type '%s' does not have the '%c' swizzle component.",
					lval->type.get_type_str().c_str(), sc));
			}
		}

		// Send the variable upwards with the swizzle applied
		NEW_EXPR_T(expr, HLSVType::MakeVectorType(ct, (uint8)stxt.length()));
		expr->text = lval->text + '.' + stxt;
		return expr;
	}
	else { // Array indexer
		// Get the nested lvalue and check
		auto lval = GET_VISIT_SPTR(ctx->LVal);
		auto ct = lval->type.get_component_type();
		auto cc = lval->type.get_component_count();

		// Check the index
		auto idx = GET_VISIT_SPTR(ctx->Index);
		if (idx->type.is_array || !idx->type.is_integer_type() || !idx->type.is_scalar_type())
			ERROR(ctx->Index, "Arrays can only be accessed using scalar non-array integer types.");

		// Generate based on the type
		HLSVType rtype{};
		if (lval->type.is_array) {
			if (idx->is_literal && idx->literal_value.ui >= lval->type.count)
				ERROR(ctx->Index, "The indexer literal is too large for the given array.");
			rtype = lval->type.type; // Same type with is_array = false
		}
		else if (lval->type.is_vector_type()) {
			if (idx->is_literal && idx->literal_value.ui >= lval->type.get_component_count())
				ERROR(ctx->Index, "The indexer literal is too large for the given vector type.");
			rtype = lval->type.get_component_type();
		}
		else if (lval->type.is_matrix_type()) {
			uint8 side = (uint8)sqrt(cc);
			if (idx->is_literal && idx->literal_value.ui >= side)
				ERROR(ctx->Index, "The indexer literal is too large for the given matrix type.");
			rtype = HLSVType::MakeVectorType(ct, side);
		}
		else
			ERROR(ctx->Index, "An array indexer is not valid for the given type.");

		// Send the variable upwards with the array indexer applied
		NEW_EXPR_T(expr, rtype);
		expr->text = lval->text + '[' + idx->text + ']';
		return expr;
	}
}

// ====================================================================================================================
VISIT_FUNC(IfStatement)
{
	// Validate the condition
	auto ifcond = GET_VISIT_SPTR(ctx->Cond);
	if (ifcond->type.is_array || ifcond->type != HLSVType::Bool)
		ERROR(ctx->Cond, "If statement conditional expressions must have a scalar boolean type.");

	// Visit the if block
	variables_.push_block(VariableManager::BT_Cond);
	gen_.emit_if_statement(*ifcond.get());
	gen_.push_indent();
	if (ctx->block()) {
		for (auto st : ctx->block()->statement())
			visit(st);
	}
	else
		visit(ctx->statement());
	gen_.pop_indent();
	gen_.emit_func_block_close();
	variables_.pop_block();

	// Visit each of the elif statements
	for (auto elif : ctx->Elifs) {
		// Validate the condition
		auto cond = GET_VISIT_SPTR(elif->Cond);
		if (cond->type.is_array || cond->type != HLSVType::Bool)
			ERROR(elif->Cond, "Elif statement conditional expressions must have a scalar boolean type.");

		// Visit the if block
		variables_.push_block(VariableManager::BT_Cond);
		gen_.emit_elif_statement(*cond.get());
		gen_.push_indent();
		if (elif->block()) {
			for (auto st : elif->block()->statement())
				visit(st);
		}
		else
			visit(elif->statement());
		gen_.pop_indent();
		gen_.emit_func_block_close();
		variables_.pop_block();
	}

	// Visit the else statement
	if (ctx->Else) {
		variables_.push_block(VariableManager::BT_Cond);
		gen_.emit_else_statement();
		gen_.push_indent();
		if (ctx->Else->block()) {
			for (auto st : ctx->Else->block()->statement())
				visit(st);
		}
		else
			visit(ctx->Else->statement());
		gen_.pop_indent();
		gen_.emit_func_block_close();
		variables_.pop_block();
	}

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(WhileLoop)
{
	// Check the condition
	auto cond = GET_VISIT_SPTR(ctx->Cond);
	if (cond->type.is_array || cond->type != HLSVType::Bool)
		ERROR(ctx->Cond, "While loop requires a scalar boolean type for its condition expression.");

	// Visit the block or statement
	variables_.push_block(VariableManager::BT_Loop);
	gen_.emit_while_loop(*cond.get());
	gen_.push_indent();
	if (ctx->block()) {
		for (auto st : ctx->block()->statement())
			visit(st);
	}
	else
		visit(ctx->statement());
	gen_.pop_indent();
	gen_.emit_func_block_close();
	variables_.pop_block();

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(DoLoop)
{
	// Check the condition
	auto cond = GET_VISIT_SPTR(ctx->Cond);
	if (cond->type.is_array || cond->type != HLSVType::Bool)
		ERROR(ctx->Cond, "While loop requires a scalar boolean type for its condition expression.");

	// Visit the block or statement
	variables_.push_block(VariableManager::BT_Loop);
	gen_.emit_do_loop();
	gen_.push_indent();
	if (ctx->block()) {
		for (auto st : ctx->block()->statement())
			visit(st);
	}
	else
		visit(ctx->statement());
	gen_.pop_indent();
	gen_.emit_do_loop_close(*cond.get());
	variables_.pop_block();

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(ForLoop)
{
	// Create the loop variable
	auto vrbl = parse_variable(ctx->Init->variableDeclaration(), VarScope::Block);
	if (vrbl.type.is_array)
		ERROR(ctx->Init, "Loop counter variables cannot be arrays.");
	if ((!vrbl.type.is_vector_type() && !vrbl.type.is_scalar_type()) || vrbl.type.get_component_type() == HLSVType::Bool)
		ERROR(ctx->Init, "Counter variables must be non-boolean scalar or vector types.");
	variables_.push_block(VariableManager::BT_Loop);
	variables_.add_variable(vrbl);

	// Check the initial value
	auto init = GET_VISIT_SPTR(ctx->Init->Value);
	if (init->type.is_array)
		ERROR(ctx->Init->Value, "Cannot initialize a counter variable with an array type.");
	if (!TypeHelper::CanPromoteTo(init->type.type, vrbl.type.type))
		ERROR(ctx->Init->Value, "The initial counter value is not a valid type.");

	// Check the condition
	auto cond = GET_VISIT_SPTR(ctx->Cond);
	if (cond->type.is_array)
		ERROR(ctx->Cond, "Loop condition cannot be an array type.");
	if (cond->type.type != HLSVType::Bool)
		ERROR(ctx->Cond, "Loop condition must be a scalar boolean type.");

	// Check the update(s)
	std::vector<string> updates{};
	for (auto up : ctx->Updates)
		updates.push_back(visit(up).as<string>());

	// Emit the header and start the new block
	gen_.emit_for_loop(vrbl, *init.get(), *cond.get(), updates);
	gen_.push_indent();

	// Visit the block, then close it
	if (ctx->block()) {
		for (auto st : ctx->block()->statement())
			visit(st);
	}
	else
		visit(ctx->statement());
	variables_.pop_block();
	gen_.pop_indent();
	gen_.emit_func_block_close();

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(ForLoopUpdate)
{
	if (ctx->Assign) { // Assignment
		auto lval = GET_VISIT_SPTR(ctx->Assign->LVal);
		auto uexpr = GET_VISIT_SPTR(ctx->Assign->Value);
		if (lval->type.is_array)
			ERROR(ctx->Assign->LVal, "Cannot assign to an array value.");
		if (uexpr->type.is_array)
			ERROR(ctx->Assign->Value, "Cannot have an array value on the right side of an assignment.");
		if (ctx->Assign->Op->getText() == "=") { // Simple assignment
			if (!TypeHelper::CanPromoteTo(uexpr->type.type, lval->type.type)) {
				ERROR(ctx->Assign->Value, strarg("The value type '%s' cannot be promoted to the variable type '%s'.",
					uexpr->type.get_type_str().c_str(), lval->type.get_type_str().c_str()));
			}
		}
		else { // Complex assignment
			string err{};
			HLSVType rtype{};
			if (!TypeHelper::CheckBinaryOperator(ctx->Assign->Op, lval->type, uexpr->type, rtype, err))
				ERROR(ctx, err);
			if (rtype != lval->type)
				ERROR(ctx->Assign->Value, "The result of the operation does not match the variable type.");
		}
		return strarg("%s %s (%s)", lval->text.c_str(), ctx->Assign->Op->getText().c_str(), uexpr->text.c_str());
	}
	else { // Unary operator
		auto lval = GET_VISIT_SPTR(ctx->LVal);
		if (lval->type.is_array || !lval->type.is_integer_type() || !lval->type.is_scalar_type())
			ERROR(ctx, strarg("Operator '%s' is only valid for non-array scalar integer variables.", ctx->Op->getText().c_str()));
		return lval->text + ctx->Op->getText();
	}
}

// ====================================================================================================================
VISIT_FUNC(ControlStatement)
{
	if (ctx->KW_BREAK()) { // 'break'
		if (!variables_.in_loop_block())
			ERROR(ctx, "'break' statement cannot be used outside of a loop block.");
		// TODO: emit
	}
	else { // 'continue'
		if (!variables_.in_loop_block())
			ERROR(ctx, "'continue' statement cannot be used outside of a loop block.");
		// TODO: emit
	}

	return nullptr;
}

} // namespace hlsv
