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
#include <vector>

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

/* ShaderStages Operators */
inline ShaderStages operator | (ShaderStages l, ShaderStages r) { return (ShaderStages)((uint8)l | (uint8)r); }
inline ShaderStages operator ^ (ShaderStages l, ShaderStages r) { return (ShaderStages)((uint8)l & ~(uint8)r); }
inline bool operator & (ShaderStages l, ShaderStages r) { return (ShaderStages)((uint8)l & (uint8)r) == r; }

// Represents a record about a specific primitive HLSV type
struct _EXPORT HLSVType final
{
public:
	// A listing of the primitive types
	enum PrimType : uint8
	{
		Void = 0, // The special "nothing" type, only valid as a function return type (void)
		Error = 255, // A value used internally to represent a type error, this value will not appear in valid shaders

		/// Scalar/Vector Value Types
		Bool  = 1, // A scalar boolean value (bool)
		Bool2 = 2, // A 2-component boolean vector (bvec2)
		Bool3 = 3, // A 3-component boolean vector (bvec3)
		Bool4 = 4, // A 4-component boolean vector (bvec4)
		Int  = 5, // A scalar 32-bit signed integer value (int)
		Int2 = 6, // A 2-component 32-bit signed integer vector (ivec2)
		Int3 = 7, // A 3-component 32-bit signed integer vector (ivec3)
		Int4 = 8, // A 4-component 32-bit signed integer vector (ivec4)
		UInt  = 9,  // A scalar 32-bit unsigned integer value (uint)
		UInt2 = 10, // A 2-component 32-bit unsigned integer vector (uvec2)
		UInt3 = 11, // A 3-component 32-bit unsigned integer vector (uvec3)
		UInt4 = 12, // A 4-component 32-bit unsigned integer vector (uvec4)
		Float  = 13, // A scalar 32-bit floating point value (float)
		Float2 = 14, // A 2-component 32-bit floating point vector (vec2)
		Float3 = 15, // A 3-component 32-bit floating point vector (vec3)
		Float4 = 16, // A 4-component 32-bit floating point vector (vec4)

		/// Matrix Value Types
		Mat2 = 150, // A square 2x2 matrix of 32-bit floating point values (mat2)
		Mat3 = 151, // A square 3x3 matrix of 32-bit floating point values (mat3)
		Mat4 = 152, // A square 4x4 matrix of 32-bit floating point values (mat4)

		/// Handle Types
		Tex1D      = 200, // A 1-dimensional combined image/sampler (tex1D)
		Tex2D      = 201, // A 2-dimensional combined image/sampler (tex2D)
		Tex3D      = 202, // A 3-dimensional combined image/sampler (tex3D)
		TexCube    = 203, // A cube-map combined image/sampler (texCube)
		Tex1DArray = 204, // An array of 1-dimensional combined image/samplers (tex1DArray)
		Tex2DArray = 205, // An array of 2-dimensional combined image/samplers (tex2DArray)
		Image1D      = 206, // A 1-dimensional non-sampled storage image (image1D)
		Image2D      = 207, // A 2-dimensional non-sampled storage image (image2D)
		Image3D      = 208, // A 3-dimensional non-sampled storage image (image3D)
		Image1DArray = 209, // An array of 1-dimensional non-sampled storage images (image1DArray)
		Image2DArray = 210, // An array of 2-dimensional non-sampled storage images (image2DArray)
	};

private:
	static const PrimType VECTOR_TYPE_START = Bool;
	static const PrimType VECTOR_TYPE_END = Float4;
	static const PrimType MATRIX_TYPE_START = Mat2;
	static const PrimType MATRIX_TYPE_END = Mat4;
	static const PrimType HANDLE_TYPE_START = Tex1D;
	static const PrimType HANDLE_TYPE_END = Image2DArray;

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

	inline bool is_error() const { return type == Error; } // Gets if the type represents a type error
	inline bool is_value_type() const { return IsValueType(type); }
	inline bool is_scalar_type() const { return IsScalarType(type); }
	inline bool is_vector_type() const { return IsVectorType(type); }
	inline bool is_matrix_type() const { return IsMatrixType(type); }
	inline bool is_handle_type() const { return IsHandleType(type); }
	inline uint8 get_component_count() const { return GetComponentCount(type); }
	inline string type_str() const { return GetTypeStr(type); }
	
	inline static bool IsValueType(enum PrimType t) {
		return (t >= VECTOR_TYPE_START && t <= VECTOR_TYPE_END) || (t >= MATRIX_TYPE_START && t <= MATRIX_TYPE_END);
	}
	inline static bool IsScalarType(enum PrimType t) {
		return (t >= VECTOR_TYPE_START && t <= VECTOR_TYPE_END) && ((t % 4) == 1);
	}
	inline static bool IsVectorType(enum PrimType t) {
		return (t >= VECTOR_TYPE_START && t <= VECTOR_TYPE_END) && ((t % 4) != 1);
	}
	inline static bool IsMatrixType(enum PrimType t) {
		return (t >= MATRIX_TYPE_START && t <= MATRIX_TYPE_END);
	}
	inline static bool IsHandleType(enum PrimType t) {
		return (t >= HANDLE_TYPE_START && t <= HANDLE_TYPE_END);
	}
	inline static uint8 GetComponentCount(enum PrimType t) {
		if (IsHandleType(t)) return 1u;
		if (IsMatrixType(t)) return (t == Mat2) ? 4u : (t == Mat3) ? 9u : 16u;
		return (((t - 1) % 4) + 1);
	}
	static string GetTypeStr(enum PrimType t);
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

// Contains information about a vertex attribute in a shader
struct _EXPORT Attribute final
{
	string name;      // The attribute name
	HLSVType type;    // The attribute type information
	uint8 location;   // The binding location of the attribute
	uint8 slot_count; // The number of binding slots taken by the attribute

	Attribute(const string& name, HLSVType type, uint8 l, uint8 sc) :
		name{ name }, type{ type }, location{ l }, slot_count{ sc }
	{ }
}; // struct Attribute

// Contains information about a fragment output in a shader
struct _EXPORT Output final
{
	string name;    // The output name
	HLSVType type;  // The output type information
	uint8 location; // The binding slot for the output

	Output(const string& name, HLSVType type, uint8 l) :
		name{ name }, type{ type }, location{ l }
	{ }
}; // struct Output

// The core reflection type that contains all reflection information about an HSLV shader
class _EXPORT ReflectionInfo final
{
public:
	uint32 tool_version;    // The version of the compiler that compiled the shader
	uint32 shader_version;  // The minimum feature version specified by the shader
	ShaderType shader_type; // The type of the shader
	ShaderStages stages;    // The stages that are present in the shader
	std::vector<Attribute> attributes; // The vertex attributes for the shader
	std::vector<Output> outputs;       // The fragment outputs for the shader

public:
	ReflectionInfo(ShaderType type, uint32 tv, uint32 sv);
	~ReflectionInfo();

	// Sorts the member vectors by binding location, info objects generated by the API will be pre-sorted
	void sort();

	inline bool is_graphics() const { return shader_type == ShaderType::Graphics; }
	inline bool is_compute() const { return shader_type == ShaderType::Compute; }
}; // class ReflectionInfo

} // namespace hlsv


// Cleanup the non-public macros
#undef _EXPORT
