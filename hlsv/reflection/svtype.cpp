/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the SVType struct from the public reflection API.
 */

#include "../hlsv-priv.hpp"
#include <map>
#include <tuple>


namespace
{

// Tuple is <hlsv str, glsl str>
extern const std::map<hlsv::uint32, std::tuple<std::string, std::string>> TYPE_NAMES;

static constexpr hlsv::uint32 prim_id_(const hlsv::SVType& type)
{
	return hlsv::uint32(type.prim) | (hlsv::uint32(type.dims[0]) << 8u) | (hlsv::uint32(type.dims[1]) << 16u);
}

} // namespace

namespace hlsv
{

// ====================================================================================================================
string SVType::hlsv_str() const
{
	auto base = std::get<0>(TYPE_NAMES.at(prim_id_(*this)));

	// Type args
	if (is_subpass_frame()) {
		base = varstr("%s<%u>", base.c_str(), uint32(extra.spf_index));
	}
	else if (is_image()) {
		auto argtype = SVType{ extra.format.prim, extra.format.count, 1, 0 };
		auto argstr = std::get<0>(TYPE_NAMES.at(prim_id_(argtype)));
		base = varstr("%s<%s>", base.c_str(), argstr.c_str());
	}

	// Array
	if (is_array()) {
		base = varstr("%s[%u]", base.c_str(), uint32(array_size));
	}

	return base;
}

// ====================================================================================================================
string SVType::glsl_str() const
{
	auto base = std::get<1>(TYPE_NAMES.at(prim_id_(*this)));

	// Array
	if (is_array()) {
		base = varstr("%s[%u]", base.c_str(), uint32(array_size));
	}

	return base;
}

} // namespace hlsv


namespace
{

const std::map<hlsv::uint32, std::tuple<std::string, std::string>> TYPE_NAMES {
	{ prim_id_(hlsv::SVTypes::Void), { "void", "void" } },
	{ prim_id_(hlsv::SVTypes::Short), { "short", "int16_t" } },
	{ prim_id_(hlsv::SVTypes::Short2), { "svec2", "i16vec2" } },
	{ prim_id_(hlsv::SVTypes::Short3), { "svec3", "i16vec3" } },
	{ prim_id_(hlsv::SVTypes::Short4), { "svec4", "i16vec4" } },
	{ prim_id_(hlsv::SVTypes::UShort), { "ushort", "uint16_t" } },
	{ prim_id_(hlsv::SVTypes::UShort2), { "usvec2", "u16vec2" } },
	{ prim_id_(hlsv::SVTypes::UShort3), { "usvec3", "u16vec3" } },
	{ prim_id_(hlsv::SVTypes::UShort4), { "usvec4", "u16vec4" } },
	{ prim_id_(hlsv::SVTypes::Int), { "int", "int" } },
	{ prim_id_(hlsv::SVTypes::Int2), { "ivec2", "ivec2" } },
	{ prim_id_(hlsv::SVTypes::Int3), { "ivec3", "ivec3" } },
	{ prim_id_(hlsv::SVTypes::Int4), { "ivec4", "ivec4" } },
	{ prim_id_(hlsv::SVTypes::UInt), { "uint", "uint" } },
	{ prim_id_(hlsv::SVTypes::UInt2), { "uivec2", "uvec2" } },
	{ prim_id_(hlsv::SVTypes::UInt3), { "uivec3", "uvec3" } },
	{ prim_id_(hlsv::SVTypes::UInt4), { "uivec4", "uvec4" } },
	{ prim_id_(hlsv::SVTypes::Long), { "long", "int64_t" } },
	{ prim_id_(hlsv::SVTypes::Long2), { "lvec2", "i64vec2" } },
	{ prim_id_(hlsv::SVTypes::Long3), { "lvec3", "i64vec3" } },
	{ prim_id_(hlsv::SVTypes::Long4), { "lvec4", "i64vec4" } },
	{ prim_id_(hlsv::SVTypes::ULong), { "ulong", "uint64_t" } },
	{ prim_id_(hlsv::SVTypes::ULong2), { "ulvec2", "u64vec2" } },
	{ prim_id_(hlsv::SVTypes::ULong3), { "ulvec3", "u64vec3" } },
	{ prim_id_(hlsv::SVTypes::ULong4), { "ulvec4", "u64vec4" } },
	{ prim_id_(hlsv::SVTypes::Float), { "float", "float" } },
	{ prim_id_(hlsv::SVTypes::Float2), { "fvec2", "vec2" } },
	{ prim_id_(hlsv::SVTypes::Float3), { "fvec3", "vec3" } },
	{ prim_id_(hlsv::SVTypes::Float4), { "fvec4", "vec4" } },
	{ prim_id_(hlsv::SVTypes::Double), { "double", "double" } },
	{ prim_id_(hlsv::SVTypes::Double2), { "dvec2", "dvec2" } },
	{ prim_id_(hlsv::SVTypes::Double3), { "dvec3", "dvec3" } },
	{ prim_id_(hlsv::SVTypes::Double4), { "dvec4", "dvec4" } },
	{ prim_id_(hlsv::SVTypes::FMat2x2), { "fmat2x2", "mat2x2" } },
	{ prim_id_(hlsv::SVTypes::FMat2x3), { "fmat2x3", "mat2x3" } },
	{ prim_id_(hlsv::SVTypes::FMat2x4), { "fmat2x4", "mat2x4" } },
	{ prim_id_(hlsv::SVTypes::FMat3x2), { "fmat3x2", "mat3x2" } },
	{ prim_id_(hlsv::SVTypes::FMat3x3), { "fmat3x3", "mat3x3" } },
	{ prim_id_(hlsv::SVTypes::FMat3x4), { "fmat3x4", "mat3x4" } },
	{ prim_id_(hlsv::SVTypes::FMat4x2), { "fmat4x2", "mat4x2" } },
	{ prim_id_(hlsv::SVTypes::FMat4x3), { "fmat4x3", "mat4x3" } },
	{ prim_id_(hlsv::SVTypes::FMat4x4), { "fmat4x4", "mat4x4" } },
	{ prim_id_(hlsv::SVTypes::DMat2x2), { "dmat2x2", "dmat2x2" } },
	{ prim_id_(hlsv::SVTypes::DMat2x3), { "dmat2x3", "dmat2x3" } },
	{ prim_id_(hlsv::SVTypes::DMat2x4), { "dmat2x4", "dmat2x4" } },
	{ prim_id_(hlsv::SVTypes::DMat3x2), { "dmat3x2", "dmat3x2" } },
	{ prim_id_(hlsv::SVTypes::DMat3x3), { "dmat3x3", "dmat3x3" } },
	{ prim_id_(hlsv::SVTypes::DMat3x4), { "dmat3x4", "dmat3x4" } },
	{ prim_id_(hlsv::SVTypes::DMat4x2), { "dmat4x2", "dmat4x2" } },
	{ prim_id_(hlsv::SVTypes::DMat4x3), { "dmat4x3", "dmat4x3" } },
	{ prim_id_(hlsv::SVTypes::DMat4x4), { "dmat4x4", "dmat4x4" } },

	{ prim_id_(hlsv::SVTypes::Tex1D), { "tex1D", "sampler1D" } },
	{ prim_id_(hlsv::SVTypes::Tex2D), { "tex2D", "sampler2D" } },
	{ prim_id_(hlsv::SVTypes::Tex3D), { "tex3D", "sampler3D" } },
	{ prim_id_(hlsv::SVTypes::TexCube), { "texCube", "samplerCube" } },
	{ prim_id_(hlsv::SVTypes::Tex1DArray), { "tex1DA", "sampler1DArray" } },
	{ prim_id_(hlsv::SVTypes::Tex2DArray), { "tex2DA", "sampler2DArray" } },
	{ prim_id_(hlsv::SVTypes::Img1D), { "img1D", "image1D" } },
	{ prim_id_(hlsv::SVTypes::Img2D), { "img2D", "image2D" } },
	{ prim_id_(hlsv::SVTypes::Img3D), { "img3D", "image3D" } },
	{ prim_id_(hlsv::SVTypes::Img1DArray), { "img1DA", "image1DArray" } },
	{ prim_id_(hlsv::SVTypes::Img2DArray), { "img2DA", "image2DArray" } },
	{ prim_id_(hlsv::SVTypes::SubpassFrame), { "frame", "subpassInput" } }
};

}
