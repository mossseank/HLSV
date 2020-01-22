/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the ReflectionInfo class from the public reflection API.
 */

#include "../hlsv-priv.hpp"


namespace hlsv
{

// ====================================================================================================================
ReflectionInfo::ReflectionInfo(PipelineType type, uint32 tv, uint32 sv) :
	type{ type },
	tool_version{ tv },
	shader_version{ sv }
{

}

// ====================================================================================================================
ReflectionInfo::~ReflectionInfo()
{

}

} // namespace hlsv
