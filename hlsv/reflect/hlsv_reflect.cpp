/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the main ReflectionInfo type from hlsv_reflect.hpp

#include "..//config.hpp"


namespace hlsv
{

// ====================================================================================================================
ReflectionInfo::ReflectionInfo(ShaderType type, uint32 tv, uint32 sv) :
	tool_version{ tv },
	shader_version{ sv },
	shader_type{ type },
	stages{ ShaderStages::None }
{

}

// ====================================================================================================================
ReflectionInfo::~ReflectionInfo()
{

}

} // namespace hlsv
