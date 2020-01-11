/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * The public API for shader program reflection and type info.
 */

#pragma once

#include "hlsv.hpp"

 /* Import/Export Macros */
#if !defined(HLSV_STATIC)
#	if defined(HLSV_MSVC)
#		if defined(_HLSV_BUILD)
#			define _EXPORT __declspec(dllexport)
#		else
#			define _EXPORT __declspec(dllimport)
#		endif // defined(_HLSV_BUILD)
#	else
#		define _EXPORT __attribute__((__visibility__("default")))
#	endif // defined(HLSV_MSVC)
#else
#	define _EXPORT
#endif // !defined(HLSV_STATIC)


namespace hlsv
{

// Contains reflection information about a shader program
class _EXPORT ReflectionInfo final
{
public:
	ReflectionInfo();
	~ReflectionInfo();
}; // class ReflectionInfo

} // namespace hlsv


#undef _EXPORT
