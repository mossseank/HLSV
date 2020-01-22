/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Declares the type which tracks information about a variable in HLSV source.
 */

#pragma once

#include "../../hlsv-priv.hpp"


namespace hlsv
{

// Variable scopes
enum class VarScope : uint8
{
	Attribute,    // Vertex attribute input
	Output,       // Fragment output
	Local,        // Inter-stage local
	Uniform,      // Uniform or uniform block member
	PushConstant, // Push constant
	Constant,     // Normal and specialization constants
	Block,        // Local within a statement block
	Builtin       // Builtin
}; // enum class VarScope

// A named/scoped/typed object with HLSV source
class Variable final
{
public:
	string name;
	string gen_name; // The name of the variable in generated GLSL
	SVType type;
	VarScope scope;
	union
	{
		struct
		{
			bool flat;
		} local; // VarScope::Local
		struct
		{
			bool spec;
			uint32 spec_index;
		} constant; // VarScope::Constant
	} extra; // Scope-specific information about the variable
	PipelineStages read;  // Bitmask of stages that can read from this variable
	PipelineStages write; // Bitmask of stages that can write to this variable

	Variable(const string& name, SVType type, VarScope scope);
	Variable(const string& name, SVType type, VarScope scope, PipelineStages read, PipelineStages write);

	inline bool is_attribute() const { return scope == VarScope::Attribute; }
	inline bool is_output() const { return scope == VarScope::Output; }
	inline bool is_local() const { return scope == VarScope::Local; }
	inline bool is_uniform() const { return scope == VarScope::Uniform; }
	inline bool is_push_constant() const { return scope == VarScope::PushConstant; }
	inline bool is_constant() const { return scope == VarScope::Constant; }
	inline bool is_spec_constant() const { return is_constant() && extra.constant.spec; }
	inline bool is_block() const { return scope == VarScope::Block; }
	inline bool is_builtin() const { return scope == VarScope::Builtin; }

	inline uint32 get_slot_count() const { return type.get_slot_count(); }

	inline bool can_read(PipelineStages stage) const { return (read != PipelineStages::None) && ((read & stage) == stage); }
	inline bool can_write(PipelineStages stage) const { return (write != PipelineStages::None) && ((write & stage) == stage); }

	inline void set_const() { write = PipelineStages::None; }

	static PipelineStages GetDefaultRead(VarScope scope);
	static PipelineStages GetDefaultWrite(VarScope scope);
}; // class Variable

} // namespace hlsv
