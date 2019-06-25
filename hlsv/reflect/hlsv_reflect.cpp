/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the main ReflectionInfo type from hlsv_reflect.hpp

#include "../config.hpp"
#include "../var/typehelper.hpp"
#include <algorithm>


namespace hlsv
{

// ====================================================================================================================
/* static */
HLSVType::PrimType HLSVType::GetComponentType(enum PrimType type)
{
	if (type == Void || type == Error || IsHandleType(type))
		return type;
	if (IsMatrixType(type))
		return Float;
	return (PrimType)((((type - 1) / 4) * 4) + 1); // This works because of the specific ordering of the value types
}

// ====================================================================================================================
/* static */
string HLSVType::GetTypeStr(enum PrimType t) 
{ 
	return TypeHelper::TypeStr(t);
}

// ====================================================================================================================
/* static */
uint32 HLSVType::GetSlotSize(HLSVType type)
{
	return TypeHelper::GetTypeSlotSize(type);
}

// ====================================================================================================================
ReflectionInfo::ReflectionInfo(ShaderType type, uint32 tv, uint32 sv) :
	tool_version{ tv },
	shader_version{ sv },
	shader_type{ type },
	stages{ ShaderStages::None },
	attributes{ },
	outputs{ }
{

}

// ====================================================================================================================
ReflectionInfo::~ReflectionInfo()
{

}

// ====================================================================================================================
void ReflectionInfo::sort()
{
	// Vertex attributes
	std::sort(attributes.begin(), attributes.end(), [](const Attribute& l, const Attribute& r) {
		return l.location < r.location;
	});

	// Outputs
	std::sort(outputs.begin(), outputs.end(), [](const Output& l, const Output& r) {
		return l.location < r.location;
	});
}

// ====================================================================================================================
uint32 ReflectionInfo::get_highest_attr_slot() const
{
	if (attributes.empty())
		return 0;

	// Cant just grab the end because this function cant assume that the vector will be sorted
	auto hi = std::max_element(attributes.begin(), attributes.end(), [](const Attribute& l, const Attribute& r) {
		return l.location < r.location;
	});
	return hi->location + hi->slot_count - 1;
}

} // namespace hlsv
