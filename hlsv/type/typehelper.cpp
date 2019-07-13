/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements typehelper.hpp

#include "typehelper.hpp"
#include "../generated/HLSV.h"
#include <cmath>

#define STR_TO_TYPE(tstr,ttype) if(str==tstr){return HLSVType::ttype;}
#define ENUM_TO_STR(ttype) if(type==HLSVType::ttype){return #ttype;}
#define TYPE_TO_STR(ttype,tstr) if(type==HLSVType::ttype){return tstr;}


namespace hlsv
{

// ====================================================================================================================
/* static */
HLSVType::PrimType TypeHelper::ParseTypeStr(const string& str)
{
	STR_TO_TYPE("void", Void)

	STR_TO_TYPE("bool", Bool) STR_TO_TYPE("bvec2", Bool2) STR_TO_TYPE("bvec3", Bool3) STR_TO_TYPE("bvec4", Bool4)
	STR_TO_TYPE("int", Int) STR_TO_TYPE("ivec2", Int2) STR_TO_TYPE("ivec3", Int3) STR_TO_TYPE("ivec4", Int4)
	STR_TO_TYPE("uint", UInt) STR_TO_TYPE("uvec2", UInt2) STR_TO_TYPE("uvec3", UInt3) STR_TO_TYPE("uvec4", UInt4)
	STR_TO_TYPE("float", Float) STR_TO_TYPE("vec2", Float2) STR_TO_TYPE("vec3", Float3) STR_TO_TYPE("vec4", Float4)

	STR_TO_TYPE("mat2", Mat2) STR_TO_TYPE("mat3", Mat3) STR_TO_TYPE("mat4", Mat4)

	STR_TO_TYPE("tex1D", Tex1D) STR_TO_TYPE("tex2D", Tex2D) STR_TO_TYPE("tex3D", Tex3D) STR_TO_TYPE("texCube", TexCube)
	STR_TO_TYPE("tex1DArray", Tex1DArray) STR_TO_TYPE("tex2DArray", Tex2DArray)
	STR_TO_TYPE("image1D", Image1D) STR_TO_TYPE("image2D", Image2D) STR_TO_TYPE("image3D", Image3D)
	STR_TO_TYPE("image1DArray", Image1DArray) STR_TO_TYPE("image2DArray", Image2DArray)
	STR_TO_TYPE("subpassInput", SubpassInput)
	
	return HLSVType::PrimType::Error;
}

// ====================================================================================================================
/* static */
string TypeHelper::TypeStr(HLSVType::PrimType type)
{
	ENUM_TO_STR(Void)

	ENUM_TO_STR(Bool) ENUM_TO_STR(Bool2) ENUM_TO_STR(Bool3) ENUM_TO_STR(Bool4)
	ENUM_TO_STR(Int) ENUM_TO_STR(Int2) ENUM_TO_STR(Int3) ENUM_TO_STR(Int4)
	ENUM_TO_STR(UInt) ENUM_TO_STR(UInt2) ENUM_TO_STR(UInt3) ENUM_TO_STR(UInt4)
	ENUM_TO_STR(Float) ENUM_TO_STR(Float2) ENUM_TO_STR(Float3) ENUM_TO_STR(Float4)

	ENUM_TO_STR(Mat2) ENUM_TO_STR(Mat3) ENUM_TO_STR(Mat4)

	ENUM_TO_STR(Tex1D) ENUM_TO_STR(Tex2D) ENUM_TO_STR(Tex3D) ENUM_TO_STR(TexCube)
	ENUM_TO_STR(Tex1DArray) ENUM_TO_STR(Tex2DArray)
	ENUM_TO_STR(Image1D) ENUM_TO_STR(Image2D) ENUM_TO_STR(Image3D)
	ENUM_TO_STR(Image1DArray) ENUM_TO_STR(Image2DArray)
	ENUM_TO_STR(SubpassInput)
	
	return "ERROR";
}

// ====================================================================================================================
/* static */
string TypeHelper::GetGLSLStr(HLSVType::PrimType type)
{
	TYPE_TO_STR(Void, "void")

	TYPE_TO_STR(Bool, "bool") TYPE_TO_STR(Bool2, "bvec2") TYPE_TO_STR(Bool3, "bvec3") TYPE_TO_STR(Bool4, "bvec4")
	TYPE_TO_STR(Int, "int") TYPE_TO_STR(Int2, "ivec2") TYPE_TO_STR(Int3, "ivec3") TYPE_TO_STR(Int4, "ivec4")
	TYPE_TO_STR(UInt, "uint") TYPE_TO_STR(UInt2, "uvec2") TYPE_TO_STR(UInt3, "uvec3") TYPE_TO_STR(UInt4, "uvec4")
	TYPE_TO_STR(Float, "float") TYPE_TO_STR(Float2, "vec2") TYPE_TO_STR(Float3, "vec3") TYPE_TO_STR(Float4, "vec4")

	TYPE_TO_STR(Mat2, "mat2") TYPE_TO_STR(Mat3, "mat3") TYPE_TO_STR(Mat4, "mat4")

	TYPE_TO_STR(Tex1D, "sampler1D") TYPE_TO_STR(Tex2D, "sampler2D") TYPE_TO_STR(Tex3D, "sampler3D") TYPE_TO_STR(TexCube, "samplerCube")
	TYPE_TO_STR(Tex1DArray, "sampler1DArray") TYPE_TO_STR(Tex2DArray, "sampler2DArray")
	TYPE_TO_STR(Image1D, "image1D") TYPE_TO_STR(Image2D, "image2D") TYPE_TO_STR(Image3D, "image3D")
	TYPE_TO_STR(Image1DArray, "image1DArray") TYPE_TO_STR(Image2DArray, "image2DArray")
	TYPE_TO_STR(SubpassInput, "subpassInput")
	
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

// ====================================================================================================================
/* static */
uint8 TypeHelper::GetValueTypeSize(HLSVType::PrimType type)
{
	if (!HLSVType::IsValueType(type))
		return 0;

	// TODO: This will need to be completely re-done once non-32-bit types are added
	return (uint16)(4 * HLSVType::GetComponentCount(type));
}

// ====================================================================================================================
/* static */
string TypeHelper::GetImageFormatStr(HLSVType::PrimType type)
{
	TYPE_TO_STR(Int, "r32i")   TYPE_TO_STR(Int2, "rg32i")   TYPE_TO_STR(Int4, "rgba32i")
	TYPE_TO_STR(UInt, "r32ui") TYPE_TO_STR(UInt2, "rg32ui") TYPE_TO_STR(UInt4, "rgba32ui")
	TYPE_TO_STR(Float, "r32f") TYPE_TO_STR(Float2, "rg32f") TYPE_TO_STR(Float4, "rgba32f")

	return "ERROR";
}

// ====================================================================================================================
/* static */
void TypeHelper::GetScalarLayoutInfo(HLSVType type, uint16* align, uint16* size)
{
	if (size) *size = (uint16)GetValueTypeSize(type.type) * (uint16)(type.is_array ? type.count : 1);

	// TODO: This will need to be completely re-done once non-32-bit types are added
	// Simple to calculate, as the scalar layout allows the alignment to be equal to the scalar component size
	if (align) *align = 4;
}

// ====================================================================================================================
/* static */
// TODO: THIS MUST BE CHANGED WHEN 64-BIT TYPES ARE ADDED
bool TypeHelper::CanPromoteTo(HLSVType::PrimType src, HLSVType::PrimType dst)
{
	if (src == dst)
		return true; // Duh
	if (src == HLSVType::Void || dst == HLSVType::Void || HLSVType::IsHandleType(src) || HLSVType::IsHandleType(dst))
		return false; // 'void' and handle types cannot ever be cast to any other type
	
	if (HLSVType::IsMatrixType(src)) {
		return src == dst; // Matrices must cast to matrices of the same size
	}
	else { // src = scalar/vector
		if (HLSVType::IsMatrixType(dst)) return false; // non-matrix -> matrix = Nope
		if (HLSVType::GetComponentCount(src) != HLSVType::GetComponentCount(dst))
			return false; // Casting cannot change the component count

		auto stype = HLSVType::GetComponentType(src);
		auto dtype = HLSVType::GetComponentType(dst);
		if (stype == HLSVType::Bool || dtype == HLSVType::Bool)
			return false; // Cannot cast to or from boolean values
		return stype <= dtype; // This only works because of the ordering of the PrimType enum
	}
}

// ====================================================================================================================
// See http://learnwebgl.brown37.net/12_shader_language/glsl_mathematical_operations.html (semi-complete)
/* static */
bool TypeHelper::CheckBinaryOperator(antlr4::Token* optk, HLSVType left, HLSVType right, HLSVType& res, string& err)
{
	// Big ol' chunky function
	using namespace grammar;
	auto op = optk->getType();
	auto optxt = optk->getText();

	// Setup initial error reporting defaults and initial checks
	res = HLSVType::Error;
	err = strarg("Invalid operand types '%s%s' %s '%s%s'", 
		left.get_type_str().c_str(), left.is_array ? strarg("[%u]", left.count).c_str() : "",
		optxt.c_str(), 
		right.get_type_str().c_str(), right.is_array ? strarg("[%u]", right.count).c_str() : "");
	if (left.is_array || right.is_array) {
		err += " - operands cannot be arrays.";
		return false;
	}
	if (!left.is_value_type() || !right.is_value_type()) {
		err += " - operands cannot be non-value types.";
		return false;
	}

	// Check the types based on the operators
	if (op == HLSV::OP_MUL) { // Multiplication ('*', probably most complex operator)
		if (left.is_boolean_type() || right.is_boolean_type()) {
			err += " - boolean types do not support multiplication.";
			return false;
		}

		if (left.is_matrix_type()) {
			if (right.is_matrix_type()) { // left = matrix, right = matrix
				if (right.get_component_count() != left.get_component_count()) {
					err += " - multiplied matrices must be the same size.";
					return false;
				}
			}
			else if (right.is_vector_type()) { // left = matrix, right = vector
				uint32 side = (uint32)sqrt(left.get_component_count());
				if (side != right.get_component_count()) {
					err += " - the right hand vector is not the correct size for the matrix.";
					return false;
				}
			}
			res = left; // Do not need to check left = matrix, right = scalar - this always succeeds
		}
		else if (left.is_vector_type()) {
			if (right.is_matrix_type()) { // left = vector, right = matrix
				err += " - invalid order for matrix/vector multiplication (matrix must come first).";
				return false;
			}
			else if (right.is_vector_type()) { // left = vector, right = matrix
				if (left.get_component_count() != right.get_component_count()) {
					err += " - cannot multiple vectors of different lengths.";
					return false;
				}
			}
			// Same thing as with matrices, multiply by scalar always succeeds
			res = HLSVType::MakeVectorType(HLSVType::GetMostPromotedType(left.type, right.type), left.get_component_count());
		}
		else { // left = scalar (always succeeds)
			if (right.is_matrix_type())
				res = right;
			else
				res = HLSVType::MakeVectorType(HLSVType::GetMostPromotedType(left.type, right.type), right.get_component_count());
		}
	}
	else if (op == HLSV::OP_DIV) { // Division ('/')
		if (left.is_boolean_type() || right.is_boolean_type()) {
			err += " - boolean types do not support division.";
			return false;
		}

		if (left.is_scalar_type()) {
			if (!right.is_scalar_type()) {
				err += " - scalars can only be divided by other scalars.";
				return false;
			}
			res = HLSVType::GetMostPromotedType(left.type, right.type);
		}
		else if (left.is_vector_type()) {
			if (right.is_matrix_type()) {
				err += " - cannot divide a vector by a matrix.";
				return false;
			}
			if (right.is_vector_type() && left.get_component_count() != right.get_component_count()) {
				err += " - can only divide vectors that are the same size.";
				return false;
			}
			res = HLSVType::MakeVectorType(HLSVType::GetMostPromotedType(left.type, right.type), left.get_component_count());
		}
		else if (left.is_matrix_type()) {
			if (!right.is_scalar_type()) {
				err += " - matrices can only be divided by scalars.";
				return false;
			}
			res = left;
		}
	}
	else if (op == HLSV::OP_MOD) { // Modulo ('%')
		if (!left.is_scalar_type() || !right.is_scalar_type() || !left.is_integer_type() || !right.is_integer_type()) {
			err += " - modulus operator requires scalar integer types.";
			return false;
		}
		res = (left == HLSVType::Int || right == HLSVType::Int) ? HLSVType::Int : HLSVType::UInt;
	}
	else if (op == HLSV::OP_ADD || op == HLSV::OP_SUB) { // Add/subtract ('+', '-')
		if (left.is_boolean_type() || right.is_boolean_type()) {
			err += " - boolean types do not support addition/subtraction.";
			return false;
		}
		if (left.get_component_count() != right.get_component_count()) {
			err += " - addition/subtraction requires types with the same number of components.";
			return false;
		}
		if (left.is_matrix_type())
			res = left;
		else
			res = HLSVType::MakeVectorType(HLSVType::GetMostPromotedType(left.type, right.type), left.get_component_count());
	}
	else if (op == HLSV::OP_LSHIFT || op == HLSV::OP_RSHIFT) { // Bit shifting ('<<', '>>')
		if (!left.is_integer_type() || !left.is_scalar_type() || !right.is_integer_type() || !right.is_scalar_type()) {
			err += " - bit shifting operations only work with scalar integers.";
			return false;
		}
		res = left;
	}
	else if (op == HLSV::OP_LT || op == HLSV::OP_GT || op == HLSV::OP_LE || op == HLSV::OP_GE) { // Relational ('<', '>', '<=', '>=')
		if (left.is_boolean_type() || right.is_boolean_type()) {
			err += " - boolean types do not support relational operators.";
			return false;
		}	
		if (!left.is_scalar_type() || !right.is_scalar_type()) {
			err += " - relational operators require scalar operands.";
			return false;
		}
		res = HLSVType::Bool;
	}
	else if (op == HLSV::OP_EQ || op == HLSV::OP_NE) { // Equality ('==', '!=')
		if (left.get_component_count() != right.get_component_count()) {
			err += " - equality operators requires types with the same number of components.";
			return false;
		}
		if (left.is_boolean_type() != right.is_boolean_type()) {
			err += " - boolean types are only equitable to other boolean types.";
			return false;
		}
		res = HLSVType::Bool;
	}
	else if (op == HLSV::OP_BITAND || op == HLSV::OP_BITOR || op == HLSV::OP_BITXOR) { // Bit logic ('&', '|', '^')
		if (!left.is_integer_type() || !left.is_scalar_type() || left != right) {
			err += " - bitwise operations only work on scalar integers of the same type.";
			return false;
		}
		res = left;
	}
	else if (op == HLSV::OP_AND || op == HLSV::OP_OR) { // Bool logic ('&&', '||')
		if (left != HLSVType::Bool || right != HLSVType::Bool) {
			err += " - both operators must be scalar booleans.";
			return false;
		}
		res = HLSVType::Bool;
	}
	else { // Unknown (error in the library, not the HLSV source)
		err += " - unknown operator '" + optxt + "'.";
		return false;
	}

	// Good to go
	return true;
}

} // namespace hlsv
