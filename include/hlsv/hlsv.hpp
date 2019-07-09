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
#define HLSV_VERSION_MAJOR 1
#define HLSV_VERSION_MINOR 0
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
#include <vector>


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

// Contains information about an error in the compiler
class _EXPORT CompilerError final
{
public:
	// Represents the sources of error in a compiler task
	enum error_source
	{
		ES_NONE = 0, // Represents no error, or an invalid CompilerError instance
		ES_FILEIO, // Represents an error during file writing or reading
		ES_PARSER, // Represents an error during the parsing process
		ES_COMPILER // Represents an error during the compilation stage, converting the HLSV to GLSL
	};

public:
	error_source source; // The source of the error
	string message; // The message explaining the nature of the error
	uint32 line; // The line that the error occured on, if the error is from the source code
	uint32 character; // The character position of the error, if the error is from the source code
	std::vector<string> rule_stack; // The grammar rule stack that generated the error, if the error is from the source code
	string bad_text; // The source text that generated the error, if applicable

	CompilerError(error_source src, const string& msg, uint32 l = 0, uint32 c = 0, const std::vector<string> & rs = {}) :
		source{ src },
		message{ msg },
		line{ l },
		character{ c },
		rule_stack{ rs },
		bad_text{ "" }
	{ }

	// Gets a string representation of the rule stack for the error
	string get_rule_stack_str() const;
}; // class CompilerError

// Forward declare the reflectioninfo type
class ReflectionInfo;

// Passes compilation options to the compile to control the compilation process
class _EXPORT CompilerOptions final
{
public:
	// Contains resource limit information about a compile process
	struct Limits
	{
		uint32 vertex_attribute_slots; // The number of vertex attribute slots available (default 16)
		uint32 fragment_outputs;       // The number of fragment outputs available (default 4)
		uint32 local_slots;            // The number of binding slots available for locals (default 8)
		uint32 uniform_sets;           // The number of uniform sets (default 4)
		uint32 uniform_bindings;       // The number of uniform bindings per uniform sets (default 8)
		uint32 uniform_block_size;     // The maximum size of any uniform block, in bytes (default 1024)
		uint32 push_constants_size;    // The maximum size of the push constants block, in bytes (default 128)
	};
	
	// The default resource limits
	static constexpr Limits DEFAULT_LIMITS = { 16, 4, 8, 4, 8, 1024, 128 };

public:
	bool generate_reflection_file; // If the reflection info file should be generated
	bool use_binary_reflection;    // If the reflection info file should be in binary instead of text
	bool keep_intermediate;	       // If the intermediate GLSL files should be kept (not deleted)
	Limits limits;                 // The resource limits to apply to the shader

public:
	CompilerOptions();
	~CompilerOptions();
}; // class CompilerOptions

// The root type for programmatically compiling HLSV shaders
class _EXPORT Compiler final
{
private:
	CompilerError last_error_;
	ReflectionInfo* reflect_;
	struct
	{
		string input_filename;
		string input_path;
		string reflection_path;
		string vert_path;
		string frag_path;
	} paths_; // Contains the absolute paths for the various compilation files

public:
	// Remove copy and move functionality
	Compiler(const Compiler&) = delete;
	Compiler& operator = (const Compiler&) = delete;
	Compiler(const Compiler&&) = delete;
	Compiler& operator = (const Compiler&&) = delete;

	Compiler();
	~Compiler();

	// Gets the last error generated by the compiler
	inline const CompilerError& get_last_error() const { return last_error_; }
	// Gets if the compiler instance has a current error
	inline bool has_error() const { return last_error_.source != CompilerError::ES_NONE; }
	// Gets the reflection info for the last call to compile(), will only be populated if the compilation was successful
	inline const ReflectionInfo& get_reflection_info() const { return *reflect_; }

	// Compiles the HLSV file with the given options, returning the success as a boolean
	// If this function returns false, then the last error will be set for the compiler instance
	bool compile(const string& file, const CompilerOptions& options);

private:
	bool preparePaths(const string& file);
	bool writeGLSL(void* gen); // void* is a strange choice, but is needed to prevent the private api from leaking into the public one
	void cleanGLSL();
}; // class Compiler

} // namespace hlsv


// Cleanup the non-public macros
#undef _EXPORT
