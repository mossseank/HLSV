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

// The types of shaders
enum class ShaderType : uint8
{
	Graphics = 0, // The shader operates in the graphics pipeline
	Compute       // The shader operates in the general compute pipeline
}; // enum class ShaderType

// Shader stages (as a bitset of flags), contains flags for all shader types, but the types shouldn't be mixed
enum class ShaderStages : uint8
{
	None = 0x00, // Represents a bitset of no stages
	/// Graphics Stages
	Vertex      = 0x01, // The vertex stage for graphics shaders
	TessControl = 0x02, // The tessellation control stage for graphics shaders
	TessEval    = 0x04, // The tessellation evaluation stage for graphics shaders
	Geometry    = 0x08, // The geometry stage for graphics shaders
	Fragment    = 0x10, // The fragment stage for graphics shaders
	MinGraphics = 0x11, // A bitset representing the minimal set of stages required for a "complete" graphics shader
	AllGraphics = 0x1F, // A bitset representing all graphics shader stages
	/// Compute Stages
	Kernel     = 0x01, // The kernel execution stage for compute shaders (the only compute shader stage)
	MinCompute = 0x01, // A bitset representing the minimal set of stages required for a "complete" compute shader
	AllCompute = 0x01  // A bitset representing all compute shader stages
}; // enum class ShaderStages

// Represents a record about a specific primitive HLSV type
struct _EXPORT HLSVType final
{
public:
	// A listing of the primitive types
	enum PrimType : uint8
	{
		Void = 0, // The special "nothing" type, only valid as a function return type
		
		/// Scalar/Vector Value Types
		Bool  = 1, // A scalar boolean value
		Bool2 = 2, // A 2-component boolean vector
		Bool3 = 3, // A 3-component boolean vector
		Bool4 = 4, // A 4-component boolean vector
		Int  = 5, // A scalar 32-bit signed integer value
		Int2 = 6, // A 2-component 32-bit signed integer vector
		Int3 = 7, // A 3-component 32-bit signed integer vector
		Int4 = 8, // A 4-component 32-bit signed integer vector
		UInt  = 9,  // A scalar 32-bit unsigned integer value 
		UInt2 = 10, // A 2-component 32-bit unsigned integer vector
		UInt3 = 11, // A 3-component 32-bit unsigned integer vector
		UInt4 = 12, // A 4-component 32-bit unsigned integer vector
		Float  = 13, // A scalar 32-bit floating point value
		Float2 = 14, // A 2-component 32-bit floating point vector
		Float3 = 15, // A 3-component 32-bit floating point vector
		Float4 = 16, // A 4-component 32-bit floating point vector

		/// Matrix Value Types
		Mat2 = 150, // A square 2x2 matrix of 32-bit floating point values
		Mat3 = 151, // A square 3x3 matrix of 32-bit floating point values
		Mat4 = 152, // A square 4x4 matrix of 32-bit floating point values

		/// Handle Types
		Tex1D      = 200, // A 1-dimensional combined image/sampler
		Tex2D      = 201, // A 2-dimensional combined image/sampler
		Tex3D      = 202, // A 3-dimensional combined image/sampler
		TexCube    = 203, // A cube-map combined image/sampler
		Tex1DArray = 204, // An array of 1-dimensional combined image/samplers
		Tex2DArray = 205, // An array of 2-dimensional combined image/samplers
		Image1D      = 206, // A 1-dimensional non-sampled storage image
		Image2D      = 207, // A 2-dimensional non-sampled storage image
		Image3D      = 208, // A 3-dimensional non-sampled storage image
		Image1DArray = 209, // An array of 1-dimensional non-sampled storage images
		Image2DArray = 210, // An array of 2-dimensional non-sampled storage images
	};

public:
	PrimType type; // The base primitive type
	bool is_array; // If the type is an array
	uint8 count;   // The number of elements in the type, will be 1 for non-arrays, and the array size for array types

public:
	HLSVType() :
		type{ Void }, is_array{ false }, count{ 1 }
	{ }
	HLSVType(enum PrimType type) :
		type{ type }, is_array{ false }, count{ 1 }
	{ }
	HLSVType(enum PrimType type, uint8 array_size) :
		type{ type }, is_array{ true }, count{ array_size }
	{ }
}; // struct HLSVType

/* HLSVType Operators */
inline bool operator == (HLSVType l, HLSVType r) {
	return l.type == r.type && l.is_array == r.is_array && l.count == r.count;
}
inline bool operator != (HLSVType l, HLSVType r) {
	return l.type != r.type || l.is_array != r.is_array || l.count != r.count;
}
inline bool operator == (HLSVType l, enum HLSVType::PrimType r) { return l.type == r; }
inline bool operator != (HLSVType l, enum HLSVType::PrimType r) { return l.type != r; }

// The core reflection type that contains all reflection information about an HSLV shader
class _EXPORT ReflectionInfo final
{
public:
	uint32 tool_version;    // The version of the compiler that compiled the shader
	uint32 shader_version;  // The minimum feature version specified by the shader
	ShaderType shader_type; // The type of the shader
	ShaderStages stages;    // The stages that are present in the shader

public:
	ReflectionInfo(ShaderType type, uint32 tv, uint32 sv);
	~ReflectionInfo();

	inline bool is_graphics() const { return shader_type == ShaderType::Graphics; }
	inline bool is_compute() const { return shader_type == ShaderType::Compute; }
}; // class ReflectionInfo

} // namespace hlsv


// Cleanup the non-public macros
#undef _EXPORT
