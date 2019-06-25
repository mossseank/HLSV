/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements visitor.hpp.

#include "visitor.hpp"
#include "var/typehelper.hpp"
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

	// Complete the full type
	auto asize = ctx->Size ? parse_size_literal(ctx->Size) : 0u;
	if (ctx->Size && asize == 0)
		ERROR(ctx->Size, "Arrays cannot have a size of zero.");
	HLSVType type = (asize == 0) ? HLSVType{ btype } : HLSVType{ btype, (uint8)asize };

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
	return { name, type, scope };
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
	// Extract the components
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
	// Extract the components
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
	// Extract the components
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

} // namespace hlsv
