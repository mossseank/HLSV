/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the public reflection API functionality 

#pragma once

#include "hlsv.hpp"

/* Import/Export Macros */
#if !defined(HLSV_STATIC)
#	if defined(HLSV_COMPILER_MSVC)
#		if defined(_HLSV_BUILD)
#			define _EXPORT __declspec(dllexport)
#		else
#			define _EXPORT __declspec(dllimport)
#		endif // defined(_HLSV_BUILD)
#	else
#		define _EXPORT __attribute__((__visibility__("default")))
#	endif // defined(HLSV_COMPILER_MSVC)
#else
#	define _EXPORT
#endif // !defined(HLSV_STATIC)


namespace hlsv
{

// The core reflection type that contains all reflection information about an HSLV shader
class _EXPORT ReflectionInfo final
{
public:
	enum ShaderType : uint8
	{
		TYPE_GRAPHICS, // The shader operates in the graphics pipeline
		TYPE_COMPUTE   // The shader operates in the general compute pipeline
	};

public:
	uint32 tool_version;    // The version of the compiler that compiled the shader
	uint32 shader_version;  // The minimum feature version specified by the shader
	ShaderType shader_type; // The type of the shader (graphics or compute)

public:
	ReflectionInfo(enum ShaderType type, uint32 tv, uint32 sv);
	~ReflectionInfo();

	inline bool is_graphics() const { return shader_type == TYPE_GRAPHICS; }
	inline bool is_compute() const { return shader_type == TYPE_COMPUTE; }
}; // class ReflectionInfo

} // namespace hlsv


// Cleanup the non-public macros
#undef _EXPORT
