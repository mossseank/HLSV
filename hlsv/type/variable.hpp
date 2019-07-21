/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the variable type at the core of the HLSV type system

#pragma once

#include "../config.hpp"
#include "typehelper.hpp"
#include "../../generated/HLSV.h"
#include <map>


namespace hlsv
{

// The different scopes the variables can have
enum class VarScope : uint8
{
	Attribute,    // Vertex attribute
	Output,	      // Fragment output
	Local,        // Inter-stage local
	Uniform,      // Uniform or uniform block member
	PushConstant, // Push constant (special uniform type)
	Constant,     // Normal constant or specialization constant
	Block,        // Variables that are local to function and statement blocks
}; // enum class VarScope

// Represents a named and scoped value object in a HLSV shader program
class Variable final
{
private:
	static std::map<string, string> BuiltinNames_;
	static ShaderType LoadedNames_;

public:
	string name;
	HLSVType type;
	VarScope scope;
	union // Used to track scope-specific information about the variable
	{
		struct
		{
			bool is_flat;
		} local;
		struct
		{
			bool is_spec;
			uint32 spec_index;
		} constant;
	};
	ShaderStages read;
	ShaderStages write;

public:
	Variable(const string& name, HLSVType type, VarScope scope);

	inline bool is_attribute() const { return scope == VarScope::Attribute; }
	inline bool is_output() const { return scope == VarScope::Output; }
	inline bool is_local() const { return scope == VarScope::Local; }
	inline bool is_uniform() const { return scope == VarScope::Uniform; }
	inline bool is_push_constant() const { return scope == VarScope::PushConstant; }
	inline bool is_constant() const { return scope == VarScope::Constant; }
	inline bool is_block() const { return scope == VarScope::Block; }

	inline uint32 get_slot_count() const { return type.get_slot_size(); }

	inline bool can_read(ShaderStages stage) const { return (scope == VarScope::Block) || read & stage; }
	inline bool can_write(ShaderStages stage) const { return (scope == VarScope::Block) || write & stage; }

	inline bool set_const() { write = ShaderStages::None; }

	static ShaderStages GetDefaultReadStages(VarScope scope);
	static ShaderStages GetDefaultWriteStages(VarScope scope);
	static void LoadNames(ShaderType type);
	static string GetOutputName(const string& name);
};

} // namespace hlsv
