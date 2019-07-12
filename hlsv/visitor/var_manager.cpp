/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements manager.hpp

#include "var_manager.hpp"
#include <numeric>


namespace hlsv
{

// ====================================================================================================================
Variable* VariableManager::VarBlock::find(const string& name)
{
	auto it = std::find_if(vars.begin(), vars.end(), [name](const Variable& var) {
		return var.name == name;
	});
	return (it != vars.end()) ? &(*it) : nullptr;
}

// ====================================================================================================================
VariableManager::VariableManager() :
	globals_{ },
	blocks_{ }
{

}

// ====================================================================================================================
VariableManager::~VariableManager()
{

}

// ====================================================================================================================
Variable* VariableManager::find_global(const string& name)
{
	auto it = std::find_if(globals_.begin(), globals_.end(), [name](const Variable& var) {
		return var.name == name;
	});
	return (it != globals_.end()) ? &(*it) : nullptr;
}

// ====================================================================================================================
Variable* VariableManager::find_variable(const string& name)
{
	for (auto it = blocks_.rbegin(); it != blocks_.rend(); ++it) {
		auto var = (*it)->find(name);
		if (var) return var;
	}
	return find_global(name);
}

// ====================================================================================================================
void VariableManager::add_global(const Variable& var)
{
	globals_.push_back(var);
}

// ====================================================================================================================
void VariableManager::add_variable(const Variable& var)
{
	blocks_.back()->vars.push_back(var);
}

// ====================================================================================================================
void VariableManager::push_block()
{
	blocks_.push_back(new VarBlock);
}

// ====================================================================================================================
void VariableManager::pop_block()
{
	delete blocks_.back();
	blocks_.pop_back();
}

// ====================================================================================================================
void VariableManager::push_stage_variables(ShaderType type, ShaderStages stage)
{
	auto it = Builtins_.find({ type, stage });
	if (it != Builtins_.end()) {
		auto bl = blocks_.back();
		for (const auto& v : it->second)
			bl->vars.push_back(v);
	}
}

// ====================================================================================================================
uint32 VariableManager::get_local_slot_count()
{
	return std::accumulate(globals_.begin(), globals_.end(), 0u, [](uint32 acc, const Variable& v) {
		return acc + (v.is_local() ? v.type.get_slot_size() : 0u);
	});
}


// ====================================================================================================================
const std::map<std::pair<ShaderType, ShaderStages>, std::vector<Variable>> VariableManager::Builtins_ = {
	{{ ShaderType::Graphics, ShaderStages::Vertex }, {
			
	}},
	{{ ShaderType::Graphics, ShaderStages::Fragment }, {

	}}
};

} // namespace hlsv
