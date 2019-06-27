/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the functions of the visitor class that are utility, as well as top level statements

#include "visitor.hpp"
#include "../var/typehelper.hpp"
#include <cstdlib>

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
Visitor::Visitor(antlr4::CommonTokenStream* ts, ReflectionInfo** refl, const CompilerOptions* opt) :
	tokens_{ ts },
	reflect_{ refl },
	options_{ opt },
	gen_{ },
	variables_{ }
{

}

// ====================================================================================================================
Visitor::~Visitor()
{

}

// ====================================================================================================================
uint32 Visitor::parse_size_literal(antlr4::Token* tk, uint32 limit)
{
	auto txt = tk->getText();
	uint64 val = std::strtoull(txt.c_str(), nullptr, 10);
	if (val >= limit) // Catches both out or range errors are parse errors
		ERROR(tk, strarg("Invalid or out of range integer value ('%s').", txt.c_str()));
	return (uint32)val;
}

// ====================================================================================================================
Variable Visitor::parse_variable(grammar::HLSV::VariableDeclarationContext* ctx, VarScope scope)
{
	// Parse the type
	auto btype = TypeHelper::ParseTypeStr(ctx->Type->getText());
	if (btype == HLSVType::Error)
		ERROR(ctx->Type, strarg("Invalid typename '%s'.", ctx->Type->getText().c_str()));
	if (btype == HLSVType::Void)
		ERROR(ctx->Type, "Variables cannot be of type 'void'.");

	// Complete the full type
	auto asize = ctx->Size ? parse_size_literal(ctx->Size) : 0u;
	if (ctx->Size && asize == 0)
		ERROR(ctx->Size, "Arrays cannot have a size of zero.");
	HLSVType vartype = (asize == 0) ? HLSVType{ btype } : HLSVType{ btype, (uint8)asize };

	// Parse the type argument (if any), and perform match checking
	auto targ = ctx->typeArgument();
	if (targ) {
		auto argstr = targ->Format ? targ->Format->getText() : targ->Index->getText();
		if (vartype.is_value_type())
			ERROR(targ, "Value types cannot have type arguments.");
		if (vartype.is_texture_type())
			ERROR(targ, "Sampled texture types cannot have type arguments.");

		if (targ->Format) { // Format = Sampled Image
			auto ifmt = TypeHelper::ParseTypeStr(targ->Format->getText());
			if (ifmt == HLSVType::Error)
				ERROR(targ->Format, strarg("Invalid format specifier '%s'.", targ->Format->getText().c_str()));
			if (!HLSVType::IsScalarType(ifmt) && !HLSVType::IsVectorType(ifmt))
				ERROR(targ->Format, "Image format type arguments must be a scalar or vector type.");
			if (HLSVType::GetComponentType(ifmt) == HLSVType::Bool)
				ERROR(targ->Format, "Image format type arguments cannot have a boolean component type.");
			if (HLSVType::GetComponentCount(ifmt) == 3)
				ERROR(targ->Format, "Image formats cannot be 3-component vectors.");

			if (!vartype.is_image_type())
				ERROR(targ->Format, "Only storage image types can have image format specifiers.");
			vartype.extra.image_format = ifmt;
		}
		else { // Index = Subpass Input
			uint32 spi = parse_size_literal(targ->Index);
			if (vartype.type != HLSVType::SubpassInput)
				ERROR(targ->Index, "Only subpass inputs can have index specifiers.");
			vartype.extra.subpass_input_index = (uint8)spi;
		}
	}
	else { // No type argument, so we need to see if one was required
		if (vartype.is_image_type())
			ERROR(ctx, "Storage images are required to have a format specifier.");
		if (vartype.type == HLSVType::SubpassInput)
			ERROR(ctx, "Subpass inputs are required to have an index specifier.");
	}

	// Parse the name
	auto name = ctx->Name->getText();
	if (name[0] == '$')
		ERROR(ctx->Name, "User-declared variables cannot start with '$'.");
	if (name.length() > 24)
		ERROR(ctx->Name, "Variable names cannot be longer than 24 characters.");

	// Check that there are not any variables with the same name
	if (variables_.find_variable(name))
		ERROR(ctx->Name, strarg("A variable with the name '%s' already exists.", name.c_str()));

	// Return the partially-complete variable
	return { name, vartype, scope };
}

// ====================================================================================================================
VISIT_FUNC(File)
{
	// Visit the version statement first
	visit(ctx->shaderVersionStatement());

	// Visit all of the top-level statements
	for (auto tls : ctx->topLevelStatement()) {
		visit(tls);
	}

	// Emit the locals
	{
		uint32 base = std::max({ REFL->get_highest_attr_slot() + 1u, (uint32)REFL->outputs.size() });
		for (const auto& loc : variables_.get_globals()) {
			if (loc.is_local()) {
				gen_.emit_local(loc, base);
				base += loc.type.get_slot_size();
			}
		}
	}

	// Sort the reflection info
	REFL->sort();

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(ShaderVersionStatement)
{
	// Extract the version and check it
	uint32 ver = std::atoi(ctx->VERSION_LITERAL()->getText().c_str());
	if (ver > HLSV_VERSION)
		ERROR(ctx, strarg("Current tool version (%u) cannot compile requested shader version (%u).", HLSV_VERSION, ver));
	if (ctx->KW_COMPUTE())
		ERROR(ctx, strarg("Compute shaders are not supported by hlsvc version %u.", HLSV_VERSION));

	// Create and populate the initial reflection info
	*reflect_ = new ReflectionInfo{ ShaderType::Graphics, HLSV_VERSION, ver };

	// No return
	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(VertexAttributeStatement)
{
	auto vdec = ctx->variableDeclaration();

	// Get the variable
	auto vrbl = parse_variable(vdec, VarScope::Attribute);
	if (!vrbl.type.is_value_type())
		ERROR(vdec->Type, "Vertex attributes must be a value type.");
	if (vrbl.type.count > 8)
		ERROR(vdec->Size, "Vertex attribute arrays cannot have more than 8 elements.");

	// Binding location information
	auto index = parse_size_literal(ctx->Index);
	if (index >= LIMITS.vertex_attribute_slots)
		ERROR(ctx->Index, strarg("Cannot bind attribute to slot %u, only %u slots available.", index, LIMITS.vertex_attribute_slots));
	auto scount = TypeHelper::GetTypeSlotSize(vrbl.type);
	if ((index + scount) > LIMITS.vertex_attribute_slots) {
		ERROR(ctx->Index, strarg("Attribute (size %u) too big for slot %u, only %u slots available.", scount, index,
			LIMITS.vertex_attribute_slots));
	}

	// Perform checks on the attribute
	for (const auto& attr : REFL->attributes) {
		bool overlap = (index == attr.location) ||
			(index < attr.location && (index + scount) > attr.location) ||
			(index > attr.location && (attr.location + (uint32)attr.slot_count) > index);
		if (overlap)
			ERROR(ctx, strarg("Attribute '%s' overlaps with existing attribute '%s'.", vrbl.name.c_str(), attr.name.c_str()));
	}

	// Attribute is good to go
	Attribute attr{ vrbl.name, vrbl.type, (uint8)index, scount };
	REFL->attributes.push_back(attr);
	variables_.add_global(vrbl);
	gen_.emit_attribute(attr);

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(FragmentOutputStatement)
{
	auto vdec = ctx->variableDeclaration();

	// Get the variable
	auto vrbl = parse_variable(vdec, VarScope::Output);
	if (!vrbl.type.is_scalar_type() && !vrbl.type.is_vector_type())
		ERROR(vdec->Type, strarg("Fragment output '%s' must be a scalar or vector type.", vrbl.name.c_str()));
	if (vrbl.type.is_array)
		ERROR(vdec->Size, strarg("Fragment output '%s' cannot be an array.", vrbl.name.c_str()));

	// Binding location information
	auto index = parse_size_literal(ctx->Index);
	if (index >= LIMITS.fragment_outputs)
		ERROR(ctx->Index, strarg("Cannot bind output to slot %u, only %u slots available.", index, LIMITS.fragment_outputs));

	// Perform checks on the output
	for (const auto& output : REFL->outputs) {
		if (output.location == index)
			ERROR(ctx, strarg("Output '%s' overlaps with existing output '%s'.", vrbl.name.c_str(), output.name.c_str()));
	}

	// Output is good to go
	Output output{ vrbl.name, vrbl.type, (uint8)index };
	REFL->outputs.push_back(output);
	variables_.add_global(vrbl);
	gen_.emit_output(output);

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(LocalStatement)
{
	auto vdec = ctx->variableDeclaration();

	// Get the variable
	auto vrbl = parse_variable(vdec, VarScope::Local);
	if (!vrbl.type.is_value_type())
		ERROR(vdec->Type, strarg("Local '%s' must be a value type.", vrbl.name.c_str()));
	vrbl.local.is_flat = !!ctx->KW_FLAT() || vrbl.type.is_integer_type();

	// Slot checking
	uint32 rem = variables_.get_local_slot_count();
	if ((rem + vrbl.get_slot_count()) > LIMITS.local_slots) {
		ERROR(ctx, strarg("Local '%s' is too large (%u slots), only %u slots still available.", vrbl.name.c_str(),
			vrbl.get_slot_count(), LIMITS.local_slots - rem));
	}

	// Local is good to go (location gets assigned later)
	variables_.add_global(vrbl);

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(UniformStatement)
{
	// Extract the binding info and check
	uint32 uset = ctx->Set ? parse_size_literal(ctx->Set) : 0u;
	uint32 ubind = parse_size_literal(ctx->Binding);
	if (uset >= LIMITS.uniform_sets)
		ERROR(ctx->Set, strarg("Uniform cannot use set %u, only %u set(s) allowed.", uset, LIMITS.uniform_sets));
	if (ubind >= LIMITS.uniform_bindings)
		ERROR(ctx->Binding, strarg("Uniform cannot use binding %u, only %u bindings(s) allowed.", ubind, LIMITS.uniform_bindings));
	auto pre = REFL->get_uniform_at(uset, ubind);
	if (pre)
		ERROR(ctx, strarg("Uniform location %u:%u is already occupied by uniform '%s'.", uset, ubind, pre->name.c_str()));

	// Split work based on block or not
	if (ctx->KW_BLOCK()) {
		auto vb = ctx->variableBlock();
		if (vb->Declarations.size() == 0)
			ERROR(ctx, "Empty uniform blocks are not allowed.");
		gen_.emit_uniform_block_header(uset, ubind);

		// Create the uniform block object
		UniformBlock ub{ (uint8)uset, (uint8)ubind };
		uint8 bindex = (uint8)REFL->blocks.size();

		// Create a new uniform for each variable in the block
		uint16 boff = 0;
		bool packed = true;
		for (auto vdec : vb->Declarations) {
			// Build the variable
			auto vrbl = parse_variable(vdec, VarScope::Uniform);
			if (!vrbl.type.is_value_type())
				ERROR(vdec->Type, "Uniforms inside of blocks must be a value type.");

			// Setup the offsets, sizes, and blocks
			uint16 ualign, usize;
			TypeHelper::GetScalarLayoutInfo(vrbl.type, &ualign, &usize);
			if ((boff % ualign) != 0) {
				boff += (ualign - (boff % ualign)); // Shift the offset to satisfy the type alignment
				packed = false;
			}

			// Add the uniform
			variables_.add_global(vrbl);
			Uniform uni{ vrbl.name, vrbl.type, (uint8)uset, (uint8)ubind, bindex, boff, usize };
			REFL->uniforms.push_back(uni);
			gen_.emit_value_uniform(uni);
			ub.members.push_back((uint8)(REFL->uniforms.size() - 1));
			boff += usize;
		}

		ub.size = boff;
		ub.packed = packed;
		REFL->blocks.push_back(ub);
		gen_.emit_uniform_block_close();
	}
	else {
		auto vdec = ctx->variableDeclaration();

		// Build the variable
		auto vrbl = parse_variable(vdec, VarScope::Uniform);
		if (!vrbl.type.is_handle_type())
			ERROR(vdec->Type, "Uniforms outside of blocks must be a handle type.");
		if (vrbl.type.is_array)
			ERROR(vdec->Size, "Handle-type uniforms cannot be arrays.");

		// Uniform-specific subpass input index checking
		if (vrbl.type == HLSVType::SubpassInput) {
			auto pre = REFL->get_subpass_input(vrbl.type.extra.subpass_input_index);
			if (pre) {
				ERROR(ctx, strarg("Subpass input index %u is already occupied by uniform '%s'.", (uint32)vrbl.type.extra.subpass_input_index,
					pre->name.c_str()));
			}
		}

		// Good to go, add the uniform
		variables_.add_global(vrbl);
		Uniform uni{ vrbl.name, vrbl.type, (uint8)uset, (uint8)ubind, 0, 0, 0 };
		REFL->uniforms.push_back(uni);
		gen_.emit_handle_uniform(uni);
	}

	return nullptr;
}

// ====================================================================================================================
VISIT_FUNC(PushConstantsStatement)
{
	auto vb = ctx->variableBlock();
	if (vb->Declarations.size() == 0)
		return nullptr;

	// Loop over the block members
	uint16 off = 0;
	bool packed = true;
	for (auto vdec : vb->Declarations) {
		// Build the variable
		auto vrbl = parse_variable(vdec, VarScope::PushConstant);
		if (!vrbl.type.is_value_type())
			ERROR(vdec->Type, "Push constants must be value types.");

		// Setup the offsets, and sizes
		uint16 ualign, usize;
		TypeHelper::GetScalarLayoutInfo(vrbl.type, &ualign, &usize);
		if ((off % ualign) != 0) {
			off += (ualign - (off % ualign)); // Shift the offset to satisfy the type alignment
			packed = false;
		}

		// Add the push constant
		variables_.add_global(vrbl);
		PushConstant pc{ vrbl.name, vrbl.type, off, usize };
		REFL->push_constants.push_back(pc);
		off += usize;
	}

	return nullptr;
}

} // namespace hlsv
