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

} // namespace hlsv
