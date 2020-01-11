/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Exposes the library configuration and primary public API.
 */

#pragma once

/* Ensure 64-bit */
#if !defined(_M_X64) && !defined(__x86_64__)
#	error HLSV can only be built for x64 platforms.
#endif

/* OS Detection */
#if defined(_WIN32)
#	define HLSV_WIN
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#elif defined(__APPLE__)
#	include "TargetConditionals.h"
#	if TARGET_OS_OSX==1
#		define HLSV_OSX
#	else
#		error Unsupported Apple OS platform, please use desktop OSX.
#	endif // TARGET_OS_OSX==1
#	define HLSV_UNIX
#elif defined(__ANDROID__)
#	error Cannot compile HLSV for Android platforms.
#elif defined(__linux__)
#	define HLSV_LINUX
#	define HLSV_UNIX
#else
#	error Supported OS not detected - please use Windows, macOSX, or desktop Linux.
#endif // defined(_WIN32)

/* Compiler Detection */
#if defined(_MSC_VER)
#	define HLSV_MSVC
#elif defined(__MINGW32__)
#	error Cannot use MinGW to compile HLSV.
#elif defined(__CYGWIN__)
#	error Cannot use Cygwin to compile HLSV.
#elif defined(__clang__)
#	define HLSV_CLANG
#elif defined(__GNUC__)
#	define HLSV_GCC
#else
#	error Unsupported compiler detected - please use MSVC, GNU GCC, or Clang.
#endif // defined(_MSC_VER)

/* Library Version */
#define HLSV_VERSION_MAJOR 1
#define HLSV_VERSION_MINOR 0
#define HLSV_VERSION_PATCH 0
#define HLSV_VERSION ((HLSV_VERSION_MAJOR*100)+(HLSV_VERSION_MINOR*10)+HLSV_VERSION_PATCH)

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

#include <cstdint>
#include <string>
#include <memory>


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
using size_t = std::size_t;

/* String */
using string = std::string;

// Represents a stage in the compiler pipeline
enum class CompilerStage
{
	FileRead, // Input files are loaded from disk
	Parse,    // HLSV source is parsed
	Compile,  // HLSV is compiled into a SPIR-V bytecode module
	FileWrite // Output files are written to the disk
}; // enum class CompilerStage

// Contains information about an error in the compilation process
class _EXPORT CompilerError final
{
public:
	CompilerStage stage; // The source stage of the error
	string message;      // The message explaining the nature of the error
	union
	{
		// Information for an error in FileRead or FileWrite stages
		struct
		{
			string file_name;  // The file that caused the file io error
		} fileio;
		// Information for an error in Parse or Compile stages
		struct
		{
			string bad_text;  // The source text that generated the error
			uint32 line;      // The line that the error occured on
			uint32 character; // The character position of the error
		} source;
	};

	CompilerError(CompilerStage stage, const string& msg) :
		stage{ stage },
		message{ msg },
		source{ "", 0, 0 }
	{ }
	~CompilerError() { }
}; // class CompilerError

// Passes compilation options to the compile to control the compilation process
class _EXPORT CompilerOptions final
{
public:
	// Shader resource count and size limit information
	struct Limits
	{
		uint32 vertex_attrib_slots; // Max slots available for vertex attributes (default 16)
		uint32 fragment_outputs;    // Max fragment output attachments (default 4)
		uint32 local_slots;         // Max slots available for locals (default 8)
		uint32 uniform_sets;        // Maximum set index for uniforms (default 4)
		uint32 uniform_bindings;    // Maximum binding index for uniforms (default 8)
		uint32 uniform_block_size;  // Maximum size in bytes of any uniform block (default 1024)
		uint32 push_block_size;     // Maximum size in bytes of the push block (default 128)
	};

	// The default resource limits
	static constexpr Limits DefaultLimits = { 16, 4, 8, 4, 8, 1024, 128 };

public:
	bool generate_reflection; // If a reflection file should be generated
	bool binary_reflection;   // If the generated reflection should use the binary format
	bool keep_intermediate;   // If the intermediate GLSL files should be kept
	Limits limits;            // The shader resource limits

	CompilerOptions() :
		generate_reflection{ false },
		binary_reflection{ false },
		keep_intermediate{ false },
		limits{ DefaultLimits }
	{ }
	~CompilerOptions() { }
}; // class CompilerOptions

// Forward declaration
class ReflectionInfo;

// Core type for programmatically compiling HLSV shaders
class _EXPORT Compiler final
{
public:
	// No copy or move functionality
	Compiler(const Compiler&) = delete;
	Compiler(Compiler&&) = delete;
	Compiler& operator = (const Compiler&) = delete;
	Compiler& operator = (Compiler&&) = delete;

	Compiler();
	~Compiler();

	// Gets the last generated compiler error.
	inline const CompilerError* get_last_error() const { return last_error_.get(); }
	// Gets if the compiler has an error.
	inline bool has_error() const { return !!last_error_.get(); }
	// Gets the reflection information of the last compilation.
	inline const ReflectionInfo* get_reflection_info() const { return reflection_.get(); }

	// Compiles the HLSV file with the given options, returning if the compilation was successful
	// If this function returns false, then the compilation error will be available
	bool compile_file(const string& path, const CompilerOptions& options);

private:
	std::unique_ptr<CompilerError> last_error_;
	std::unique_ptr<ReflectionInfo> reflection_;
	struct
	{
		string input_path;
		string input_file;
		string reflection_path;
		string output_path;
	} paths_;
}; // class Compiler

} // namespace hlsv


#undef _EXPORT
