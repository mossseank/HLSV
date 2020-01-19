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

// ====================================================================================================================
// == TYPE DECLARATIONS ==
// ====================================================================================================================

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

// Forward declarations
enum class CompilerStage;
class CompilerError;
class CompilerOptions;
class Compiler;
class ShaderType;
class ReflectionInfo;
struct Attribute;
struct Fragment;
struct Uniform;
struct UniformBlock;
struct PushConstant;
struct SpecConstant;


// ====================================================================================================================
// == Compiler Types ==
// ====================================================================================================================

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

// Passes compilation options to the compiler to control the compilation process
class _EXPORT CompilerOptions final
{
public:
	// Shader resource count and size limit information
	struct Limits final
	{
		uint32 vertex_attrib_slots; // Max slots available for vertex attributes (default 16)
		uint32 fragment_outputs;    // Max fragment output attachments (default 4)
		uint32 local_slots;         // Max slots available for locals (default 8)
		uint32 uniform_sets;        // Maximum set index for uniforms (default 4)
		uint32 uniform_bindings;    // Maximum binding index for uniforms (default 8)
		uint32 uniform_block_size;  // Maximum size in bytes of any uniform block (default 1024)
		uint32 push_block_size;     // Maximum size in bytes of the push block (default 128)
	};
	// Shader features
	struct Features final
	{
		bool int16;   // Support 16-bit integers (default true)
		bool int64;   // Support 64-bit integers (default true)
		bool float64; // Support 64-bit floats (default true)
	}; // struct Features

public:
	// The default resource limits
	static constexpr Limits DefaultLimits { 16, 4, 8, 4, 8, 1024, 128 };
	// The default shader features
	static constexpr Features DefaultFeatures { true, true, true };
	
	bool generate_reflection; // If a reflection file should be generated
	bool binary_reflection;   // If the generated reflection should use the binary format
	bool keep_intermediate;   // If the intermediate GLSL files should be kept
	string output_path;       // The override path for the spirv bytecode file
	string reflection_path;   // The override path for the shader reflection file
	Limits limits;            // The shader resource limits
	Features features;  // The allowed shader features

	CompilerOptions() :
		generate_reflection{ false },
		binary_reflection{ false },
		keep_intermediate{ false },
		output_path{ "" },
		reflection_path{ "" },
		limits{ DefaultLimits },
		features{ DefaultFeatures }
	{ }
	~CompilerOptions() { }
}; // class CompilerOptions

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

	bool prepare_paths(const string& path, const CompilerOptions& options);

	bool ERROR(CompilerStage stage, const string& msg, const string& path);
	bool ERROR(CompilerStage stage, const string& msg, const string& bad, uint32 line, uint32 cpos);
}; // class Compiler


// ====================================================================================================================
// == Reflection Types ==
// ====================================================================================================================

// Extra shader features (redeclare)
using ShaderFeatures = CompilerOptions::Features;

// The types of shader pipeline (acts as a bitset)
enum class PipelineType : uint8
{
	Graphics = 0, // Standard graphics pipeline
	Compute  = 1, // Compute pipeline
}; // enum class PipelineType

// A bitmask enumeration of pipeline stages, containing flags for all pipeline types
enum class PipelineStages : uint8
{
	None        = 0x00, // A bitmask of no stages
	// Graphics Stages
	Vertex      = 0x01, // The vertex stage for graphics pipelines
	TessControl = 0x02, // The tesselation control stage for graphics pipelines
	TessEval    = 0x04, // The tesselation evaluation stage for graphics pipelines
	Geometry    = 0x08, // The geometry stage for graphics pipelines
	Fragment    = 0x10, // The fragment stage for graphics pipelines
	MinGraphics = 0x11, // A bitmask of the minimum set of required graphics stages (Vertex and Fragment)
	AllGraphics = 0x1F, // A bitmask of all graphics pipeline stages
	// Compute Stages
	Kernel      = 0x01, // The kernel execution stage for compute pipelines
	AllCompute  = 0x01, // A bitmask of all compute pipeline stages
}; // enum class PipelineStages

// PipelineStages operators
inline PipelineStages operator | (PipelineStages l, PipelineStages r) { 
	return (PipelineStages)((uint8)l | (uint8)r);
}
inline PipelineStages operator & (PipelineStages l, PipelineStages r) {
	return (PipelineStages)((uint8)l & (uint8)r);
}
inline PipelineStages operator ^ (PipelineStages l, PipelineStages r) {
	return (PipelineStages)((uint8)l & ~(uint8)r);
}

// Contains type information about an object or value in HLSV
struct _EXPORT SVType final
{
public:
	enum class PrimType : uint8
	{
		Void = 0, // Special "nothing" type

		/// Value types
		Bool    = 1, // Boolean (bool, bvec2, bvec3, bvec4)
		Short   = 2, // Signed 16-bit integer (short, svec2, svec3, svec4)
		UShort  = 3, // Unsigned 16-bit integer (ushort, usvec2, usvec3, usvec4)
		Int     = 4, // Signed 32-bit integer (int, ivec2, ivec3, ivec4)
		UInt    = 5, // Unsigned 32-bit integer (uint, uivec2, uivec3, uivec4)
		Long    = 6, // Signed 64-bit integer (long, lvec2, lvec3, lvec4)
		ULong   = 7, // Unsigned 64-bit integer (ulong, ulvec2, ulvec3, ulvec4)
		Float   = 8, // 32-bit float (float, fvec2, fvec3, fvec4, fmat*)
		Double  = 9, // 64-bit float (double, dvec2, dvec3, dvec4, dmat*)

		/// Handle Types
		Tex1D         = 128, // Dim-1 combined image/sampler (tex1D)
		Tex2D         = 129, // Dim-2 combined image/sampler (tex2D)
		Tex3D         = 130, // Dim-3 combined image/sampler (tex3D)
		TexCube       = 131, // Cube-mapped combined image/sampler (texCube)
		Tex1DArray    = 132, // Array of dim-1 combined image/samplers (tex1DArray)
		Tex2DArray    = 133, // Array of dim-2 combined image/samplers (tex2DArray)
		Img1D         = 134, // Dim-1 storage image (img1D)
		Img2D         = 135, // Dim-2 storage image (img2D)
		Img3D         = 136, // Dim-3 storage image (img3D)
		Img1DArray    = 137, // Array of dim-1 storage images (img1DArray)
		Img2DArray    = 138, // Array of dim-2 storage images (img2DArray)
		SubpassBuffer = 139, // Vulkan subpass input texture resource (spBuffer)
	}; // enum PrimType

public:
	PrimType prim;     // The primitive backing type
	uint8 dims[2];     // The dimensions of the backing type (value types only)
	union
	{
		uint16 spi_index; // The subpass input index of the type (if prim == SubpassBuffer)
		struct
		{
			PrimType prim;
			uint8 count;
		} format;         // The format of the storage image (if prim == Img*)
	} extra;		   // Type-specific extra information (must stay at 2 bytes large)
	uint16 array_size; // The size of the array, or 0 if not an array type

	constexpr SVType() :
		prim{ PrimType::Void }, dims{ 0, 0 }, array_size{ 0 }, extra{ 0 }
	{ }
	constexpr SVType(PrimType type) :
		prim{ type }, dims{ 1, 1 }, array_size{ 0 }, extra{ 0 }
	{ }
	constexpr SVType(PrimType type, uint8 dim1, uint8 dim2, uint16 arr) :
		prim{ type }, dims{ dim1, dim2 }, array_size{ arr }, extra{ 0 }
	{ }

	constexpr inline bool operator == (const SVType& o) const {
		return (prim == o.prim) && (dims[0] == o.dims[0]) && (dims[1] == o.dims[1]) &&
			   (array_size == o.array_size) && (extra.spi_index == o.extra.spi_index);
	}
	constexpr inline bool operator != (const SVType& o) const {
		return (prim != o.prim) || (dims[0] != o.dims[0]) || (dims[1] != o.dims[1]) ||
			   (array_size != o.array_size) || (extra.spi_index != o.extra.spi_index);
	}

	inline bool is_void() const { return prim == PrimType::Void; }
	inline bool is_value() const { return prim >= PrimType::Bool && prim <= PrimType::Double; }
	inline bool is_handle() const { return prim >= PrimType::Tex1D && prim <= PrimType::SubpassBuffer; }
	inline bool is_scalar() const { return is_value() && dims[0] == 1 && dims[1] == 1; }
	inline bool is_vector() const { return is_value() && dims[0] > 1 && dims[1] == 1; }
	inline bool is_matrix() const { return is_value() && dims[0] > 1 && dims[1] > 1; }
	inline bool is_texture() const { return prim >= PrimType::Tex1D && prim <= PrimType::Tex2DArray; }
	inline bool is_image() const { return prim >= PrimType::Img1D && prim <= PrimType::Img2DArray; }
	inline bool is_subpass_buffer() const { return prim == PrimType::SubpassBuffer; }
	inline bool is_integer() const { return prim >= PrimType::Short && prim <= PrimType::ULong; }
	inline bool is_fpoint() const { return prim == PrimType::Float || prim == PrimType::Double; }
	inline bool is_boolean() const { return prim == PrimType::Bool; }
	inline bool is_array() const { return array_size > 0; }

	inline uint8 get_primitive_size() const {
		return (prim == PrimType::Short || prim == PrimType::UShort) ? 2 :
			   (prim == PrimType::Long || prim == PrimType::ULong || prim == PrimType::Double) ? 8 : 4;
	}
	inline uint8 get_row_size() const { return (uint8)(dims[0] * get_primitive_size()); }
	inline uint8 get_component_count() const { return (uint8)(dims[0] * dims[1]); }
	inline uint16 get_slot_count() const { return (uint16)(dims[1] * ((get_row_size() > 16) ? 2 : 1) * ((array_size == 0) ? 1 : array_size)); }

	string str() const;
}; // struct SVType

// Contains information about a vertex attribute in a shader
struct _EXPORT Attribute final
{
	string name;      // The attribute name
	SVType type;      // The attribute type information
	uint8 location;   // The binding location of the attribute

	Attribute(const string& name, SVType type, uint8 l) :
		name{ name }, type{ type }, location{ l }
	{ }
}; // struct Attribute

// Contains information about a fragment output in a shader
struct _EXPORT Fragment final
{
	string name;    // The output name
	SVType type;    // The output type information
	uint8 location; // The binding slot for the output

	Fragment(const string& name, SVType type, uint8 l) :
		name{ name }, type{ type }, location{ l }
	{ }
}; // struct Fragment

// Contains information about a shader uniform
struct _EXPORT Uniform final
{
	string name;
	SVType type;
	uint8 set;
	uint8 binding;
	struct
	{
		uint8 index;   // The index of the uniform block that this uniform belongs to, if applicable
		uint16 offset; // The offset of the uniform within its block, in bytes
	} block; // Contains block information, only valid for value-type uniforms inside of blocks

	Uniform(const string& name, SVType type, uint8 s, uint8 b, uint8 bl, uint16 o) :
		name{ name }, type{ type }, set{ s }, binding{ b }, block{ bl, o }
	{ }
}; // struct Uniform

// Contains information about a shader uniform block
struct _EXPORT UniformBlock final
{
	uint8 set;
	uint8 binding;
	uint16 size;				// Total size of the block in bytes
	bool packed;				// If the members in the block are tightly packed

	UniformBlock(uint8 s, uint8 b) :
		set{ s }, binding{ b }, size{ 0 }, packed{ false }
	{ }
}; // struct UniformBlock

// Contains information about a push constant
struct _EXPORT PushConstant final
{
	string name;
	SVType type;
	uint16 offset;

	PushConstant(const string& name, SVType type, uint16 o) :
		name{ name }, type{ type }, offset{ o }
	{ }
}; // struct PushConstant

// Contains information about a specialization constant
struct _EXPORT SpecConstant final
{
	string name;
	SVType type;
	uint8 index;
	union
	{
		double f;   // The default floating point value
		int64 si;   // The default signed integer value
		uint64 ui;  // The default unsigned integer value
	} default_value; // The default value for the spec constant

	SpecConstant(const string& name, SVType type, uint8 i) :
		name{ name }, type{ type }, index{ i }, default_value{ 0ull }
	{ }
}; // struct SpecConstant

// Contains reflection information about a shader program
class _EXPORT ReflectionInfo final
{
public:
	ReflectionInfo();
	~ReflectionInfo();
}; // class ReflectionInfo

} // namespace hlsv


#undef _EXPORT
