/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements manager.hpp

#include "manager.hpp"


namespace hlsv
{

// ====================================================================================================================
VariableManager::VariableManager() :
	globals_{ }
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
	return find_global(name); // TODO: search the scope stack when implemented
}

// ====================================================================================================================
void VariableManager::add_global(const Variable& var)
{
	globals_.push_back(var);
}

} // namespace hlsv
