/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements the SVType struct from the public reflection API.
 */

#include "../hlsv-priv.hpp"
#include <vector>


namespace hlsv
{

// ====================================================================================================================
string SVType::str() const
{
	static const std::vector<std::vector<const char*>> VALUE_STR {
		{ "void" },
		{ "bool", "bvec2", "bvec3", "bvec4" },
		{ "short", "svec2", "svec3", "svec4" },
		{ "ushort", "usvec2", "usvec3", "usvec4" },
		{ "int", "ivec2", "ivec3", "ivec4" },
		{ "uint", "uivec2", "uivec3", "uivec4" },
		{ "long", "lvec2", "lvec3", "lvec4" },
		{ "ulong", "ulvec2", "ulvec3", "ulvec4" },
		{ "float", "fvec2", "fvec3", "fvec4" },
		{ "double", "dvec2", "dvec3", "dvec4" }
	};
	static const std::vector<const char*> HANDLE_STR {
		"tex1D", "tex2D", "tex3D", "texCube", "tex1DArray", "tex2DArray",
		"img1D", "img2D", "img3D", "img1DArray", "img2DArray",
		"spBuffer"
	};

	string base{ is_value() ? VALUE_STR[(int)prim][dims[0]-1] : HANDLE_STR[(int)prim - (int)PrimType::Tex1D] };
	if (is_image()) {
		base = varstr("%s<%s>", base.c_str(), VALUE_STR[(int)extra.format.prim][extra.format.count-1]);
	}
	else if (is_subpass_buffer()) {
		base = varstr("%s<%d>", base.c_str(), (uint32)extra.spi_index);
	}
	return is_array() ? varstr("%s[%d]", base.c_str(), (uint32)array_size) : base;
}

} // namespace hlsv
