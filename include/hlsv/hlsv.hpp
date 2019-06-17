/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file is the core header and primary public API for the hlsv library. It performs all of the standard includes,
//    configurations, type forwards, and macro definitions used in the public API. It additionally declares all of the
//    types and functionality that are available in the public API.

#pragma once


/* Ensure 64-bit */
#if !defined(_M_X64) && !defined(__x86_64__)
#	error HLSV can only be built for x64 platforms.
#endif

/* OS Detection */
#if defined(_WIN32)
#	define HLSV_OS_WIN
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#elif defined(__APPLE__)
#	include "TargetConditionals.h"
#	if TARGET_OS_OSX==1
#		define HLSV_OS_OSX
#	else
#		error Unsupported Apple OS platform, please use desktop OSX.
#	endif // TARGET_OS_OSX==1
#	define HLSV_OS_UNIX
#elif defined(__ANDROID__)
#	error Cannot compile HLSV for Android platforms.
#elif defined(__linux__)
	// Technically android is linux too, but that case is caught above and errored out
#	define HLSV_OS_LINUX
#	define HLSV_OS_UNIX
#else
#	error Supported OS not detected - please use Windows, macOSX, or desktop Linux.
#endif // defined(_WIN32)

/* Compiler Detection */
#if defined(_MSC_VER)
#	define HLSV_COMPILER_MSVC
#elif defined(__MINGW32__)
#	error Cannot use MinGW to compile HLSV.
#elif defined(__CYGWIN__)
#	error Cannot use Cygwin to compile HLSV.
#elif defined(__clang__)
#	define HLSV_COMPILER_CLANG
#elif defined(__GNUC__)
#	define HLSV_COMPILER_GCC
#else
#	error Unsupported compiler detected - please use MSVC, GNU GCC, or Clang.
#endif // defined(_MSC_VER)

/* Library Version */
#define HLSV_VERSION_MAJOR 0
#define HLSV_VERSION_MINOR 1
#define HLSV_VERSION_PATCH 0
// This is a 3-digit version in the form MajorMinorPatch, similar to how GLSL defines its versions
#define HLSV_VERSION ((HLSV_VERSION_MAJOR*100)+(HLSV_VERSION_MINOR*10)+HLSV_VERSION_PATCH)

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


#include <cstdint>
#include <string>


namespace hlsv
{

/* Integer Types */
using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8   = std::int8_t;
using int16  = std::int16_t;
using int32  = std::int32_t;
using int64  = std::int64_t;

/* String */
using string = std::string;

/* String Formatting */
// Creates an std::string instance using printf style formatting
_EXPORT string strarg(const char* const fmt, ...);

// The root type for programmatically compiling HLSV shaders
class _EXPORT Compiler final
{
public:
	// Remove copy and move functionality
	Compiler(const Compiler&) = delete;
	Compiler& operator = (const Compiler&) = delete;
	Compiler(const Compiler&&) = delete;
	Compiler& operator = (const Compiler&&) = delete;

	Compiler();
	~Compiler();

	// Compiles the HLSV file with the given options, returning the success as a boolean
	bool compile(const string& file);
}; // class Compiler

} // namespace hlsv


// Cleanup the non-public macros
#undef _EXPORT
