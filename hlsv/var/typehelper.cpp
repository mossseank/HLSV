/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements typehelper.hpp

#include "typehelper.hpp"

#define STR_TO_TYPE(tstr,ttype) if(str==tstr){return HLSVType::ttype;}
#define TYPE_TO_STR(ttype) if(type==HLSVType::ttype){return #ttype;}


namespace hlsv
{

// ====================================================================================================================
/* static */
HLSVType::PrimType TypeHelper::ParseTypeStr(const string& str)
{
	STR_TO_TYPE("void", Void)

	STR_TO_TYPE("bool", Bool)
	STR_TO_TYPE("bvec2", Bool2)
	STR_TO_TYPE("bvec3", Bool3)
	STR_TO_TYPE("bvec4", Bool4)
	STR_TO_TYPE("int", Int)
	STR_TO_TYPE("ivec2", Int2)
	STR_TO_TYPE("ivec3", Int3)
	STR_TO_TYPE("ivec4", Int4)
	STR_TO_TYPE("uint", UInt)
	STR_TO_TYPE("uvec2", UInt2)
	STR_TO_TYPE("uvec3", UInt3)
	STR_TO_TYPE("uvec4", UInt4)
	STR_TO_TYPE("float", Float)
	STR_TO_TYPE("vec2", Float2)
	STR_TO_TYPE("vec3", Float3)
	STR_TO_TYPE("vec4", Float4)

	STR_TO_TYPE("mat2", Mat2)
	STR_TO_TYPE("mat3", Mat3)
	STR_TO_TYPE("mat4", Mat4)

	STR_TO_TYPE("tex1D", Tex1D)
	STR_TO_TYPE("tex2D", Tex2D)
	STR_TO_TYPE("tex3D", Tex3D)
	STR_TO_TYPE("texCube", TexCube)
	STR_TO_TYPE("tex1DArray", Tex1DArray)
	STR_TO_TYPE("tex2DArray", Tex2DArray)
	STR_TO_TYPE("image1D", Image1D)
	STR_TO_TYPE("image2D", Image2D)
	STR_TO_TYPE("image3D", Image3D)
	STR_TO_TYPE("image1DArray", Image1DArray)
	STR_TO_TYPE("image2DArray", Image2DArray)
	
	return HLSVType::PrimType::Error;
}

// ====================================================================================================================
/* static */
string TypeHelper::TypeStr(HLSVType::PrimType type)
{
	TYPE_TO_STR(Void)

	TYPE_TO_STR(Bool)
	TYPE_TO_STR(Bool2)
	TYPE_TO_STR(Bool3)
	TYPE_TO_STR(Bool4)
	TYPE_TO_STR(Int)
	TYPE_TO_STR(Int2)
	TYPE_TO_STR(Int3)
	TYPE_TO_STR(Int4)
	TYPE_TO_STR(UInt)
	TYPE_TO_STR(UInt2)
	TYPE_TO_STR(UInt3)
	TYPE_TO_STR(UInt4)
	TYPE_TO_STR(Float)
	TYPE_TO_STR(Float2)
	TYPE_TO_STR(Float3)
	TYPE_TO_STR(Float4)

	TYPE_TO_STR(Mat2)
	TYPE_TO_STR(Mat3)
	TYPE_TO_STR(Mat4)

	TYPE_TO_STR(Tex1D)
	TYPE_TO_STR(Tex2D)
	TYPE_TO_STR(Tex3D)
	TYPE_TO_STR(TexCube)
	TYPE_TO_STR(Tex1DArray)
	TYPE_TO_STR(Tex2DArray)
	TYPE_TO_STR(Image1D)
	TYPE_TO_STR(Image2D)
	TYPE_TO_STR(Image3D)
	TYPE_TO_STR(Image1DArray)
	TYPE_TO_STR(Image2DArray)
	
	return "ERROR";
}

// ====================================================================================================================
/* static */
uint8 TypeHelper::GetPrimitiveSlotCount(HLSVType::PrimType type)
{
	if (HLSVType::IsHandleType(type)) return 1;
	if (HLSVType::IsScalarType(type)) return 1;
	if (HLSVType::IsVectorType(type)) return 1; // TODO: this will not hold true for 64-bit values
	return (type == HLSVType::Mat2) ? 2u : (type == HLSVType::Mat3) ? 3u : 4u;
}

} // namespace hlsv
