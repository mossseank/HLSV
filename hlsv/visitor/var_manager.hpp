/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the variable manager, which manages variable scopes 

#pragma once

#include "../config.hpp"
#include "../type/variable.hpp"
#include "../type/typehelper.hpp"
#include <vector>
#include <map>


namespace hlsv
{

// Manages the variable scope stack
class VariableManager final
{
	using varvec = std::vector<Variable>;

	class VarBlock final
	{
	public:
		varvec vars;

		VarBlock() : vars{ } { }

		Variable* find(const string& name);
	}; // class VarBlock

private:
	static const std::map<std::pair<ShaderType, ShaderStages>, std::vector<Variable>> Builtins_;
	varvec globals_; // The global variables (all that dont exist in any local scopes)
	std::vector<VarBlock*> blocks_;

public:
	VariableManager();
	~VariableManager();

	Variable* find_global(const string& name);
	Variable* find_variable(const string& name);
	void add_global(const Variable& var); // Does not check if a variable with the name already exists
	void add_variable(const Variable& var); // Same

	void push_block();
	void pop_block();

	void push_stage_variables(ShaderType type, ShaderStages stage);

	inline const varvec& get_globals() const { return globals_; }

	uint32 get_local_slot_count(); // The number of slots currently taken by locals
}; // class VariableManager

} // namespace hlsv