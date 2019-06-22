/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements typehelper.hpp

#include "typehelper.hpp"

#define TYPE_STR(tstr,ttype) if(str==tstr){return HLSVType::ttype;}


namespace hlsv
{

// ====================================================================================================================
/* static */
HLSVType::PrimType TypeHelper::ParseTypeStr(const string& str)
{
	TYPE_STR("void", Void)

	TYPE_STR("bool", Bool)
	TYPE_STR("bvec2", Bool2)
	TYPE_STR("bvec3", Bool3)
	TYPE_STR("bvec4", Bool4)
	TYPE_STR("int", Int)
	TYPE_STR("ivec2", Int2)
	TYPE_STR("ivec3", Int3)
	TYPE_STR("ivec4", Int4)
	TYPE_STR("uint", UInt)
	TYPE_STR("uvec2", UInt2)
	TYPE_STR("uvec3", UInt3)
	TYPE_STR("uvec4", UInt4)
	TYPE_STR("float", Float)
	TYPE_STR("vec2", Float2)
	TYPE_STR("vec3", Float3)
	TYPE_STR("vec4", Float4)

	TYPE_STR("mat2", Mat2)
	TYPE_STR("mat3", Mat3)
	TYPE_STR("mat4", Mat4)

	TYPE_STR("tex1D", Tex1D)
	TYPE_STR("tex2D", Tex2D)
	TYPE_STR("tex3D", Tex3D)
	TYPE_STR("texCube", TexCube)
	TYPE_STR("tex1DArray", Tex1DArray)
	TYPE_STR("tex2DArray", Tex2DArray)
	TYPE_STR("image1D", Image1D)
	TYPE_STR("image2D", Image2D)
	TYPE_STR("image3D", Image3D)
	TYPE_STR("image1DArray", Image1DArray)
	TYPE_STR("image2DArray", Image2DArray)
	
	return HLSVType::PrimType::Error;
}

} // namespace hlsv
