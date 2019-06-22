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


namespace hlsv
{

// The different scopes the variables can have
enum class VarScope : uint8
{
	Attribute, // Vertex attribute
}; // enum class VarScope

// Represents a named and scoped value object in a HLSV shader program
class Variable final
{
public:
	string name;
	HLSVType type;
	VarScope scope;

public:
	Variable(const string& name, HLSVType type, VarScope scope);

	inline bool is_attribute() const { return scope == VarScope::Attribute; }
};

} // namespace hlsv
