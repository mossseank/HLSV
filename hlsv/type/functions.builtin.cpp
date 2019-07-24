/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

 // This file implements the registration of builtin funtions to the FunctionRegistry

#include "functions.hpp"

#define IM_STR(it, tt, ct) { "imageStore", HLSVType::Void, { HLSVType(HLSVType::it, HLSVType::tt), { HLSVType::ct, false }, { HLSVType::tt, false } } }


namespace hlsv
{

// ====================================================================================================================
/* static */
// Built off of https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.50.pdf, section 8 on built-in functions
void FunctionRegistry::Populate()
{
	Functions_.insert({
		// Angle and trigonometry functions
		{ "d2r", {
			{ "radians", 0, { HLSVType::Float } }
		}},
		{ "r2d", {
			{ "degrees", 0, { HLSVType::Float } }
		}},
		{ "sin", {
			{ "sin", 0, { HLSVType::Float } }
		}},
		{ "cos", {
			{ "cos", 0, { HLSVType::Float } }
		}},
		{ "tan", {
			{ "tan", 0, { HLSVType::Float } }
		}},
		{ "asin", {
			{ "asin", 0, { HLSVType::Float } }
		}},
		{ "acos", {
			{ "acos", 0, { HLSVType::Float } }
		}},
		{ "atan", {
			{ "atan", 0, { HLSVType::Float } }
		}},
		{ "atan2", {
			{ "atan", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "sinh", {
			{ "sinh", 0, { HLSVType::Float } }
		}},
		{ "cosh", {
			{ "cosh", 0, { HLSVType::Float } }
		}},
		{ "tanh", {
			{ "tanh", 0, { HLSVType::Float } }
		}},
		{ "asinh", {
			{ "asinh", 0, { HLSVType::Float } }
		}},
		{ "acosh", {
			{ "acosh", 0, { HLSVType::Float } }
		}},
		{ "atanh", {
			{ "atanh", 0, { HLSVType::Float } }
		}},
		// Exponential functions
		{ "pow", {
			{ "pow", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "exp", {
			{ "exp", 0, { HLSVType::Float } }
		}},
		{ "log", {
			{ "log", 0, { HLSVType::Float } }
		}},
		{ "exp2", {
			{ "exp2", 0, { HLSVType::Float } }
		}},
		{ "log2", {
			{ "log2", 0, { HLSVType::Float } }
		}},
		{ "sqrt", {
			{ "sqrt", 0, { HLSVType::Float } }
		}},
		{ "isqrt", {
			{ "inversesqrt", 0, { HLSVType::Float } }
		}},
		// Common functions
		{ "abs", {
			{ "abs", 0, { HLSVType::Int }},
			{ "abs", 0, { HLSVType::Float }}
		}},
		{ "sign", {
			{ "sign", 0, { HLSVType::Int }},
			{ "sign", 0, { HLSVType::Float }}
		}},
		{ "floor", {
			{ "floor", 0, { HLSVType::Float } }
		}},
		{ "trunc", {
			{ "trunc", 0, { HLSVType::Float } }
		}},
		{ "round", {
			{ "round", 0, { HLSVType::Float } }
		}},
		{ "roundEven", {
			{ "roundEven", 0, { HLSVType::Float } }
		}},
		{ "ceil", {
			{ "ceil", 0, { HLSVType::Float } }
		}},
		{ "fract", {
			{ "fract", 0, { HLSVType::Float } }
		}},
		{ "mod", {
			{ "mod", 0, { HLSVType::Float, { HLSVType::Float, false } } },
			{ "mod", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "min", {
			{ "min", 0, { HLSVType::Int, { HLSVType::Int, false } } },
			{ "min", 0, { HLSVType::Int, HLSVType::Int } },
			{ "min", 0, { HLSVType::UInt, { HLSVType::UInt, false } } },
			{ "min", 0, { HLSVType::UInt, HLSVType::UInt } },
			{ "min", 0, { HLSVType::Float, { HLSVType::Float, false } } },
			{ "min", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "max", {
			{ "max", 0, { HLSVType::Int, { HLSVType::Int, false } } },
			{ "max", 0, { HLSVType::Int, HLSVType::Int } },
			{ "max", 0, { HLSVType::UInt, { HLSVType::UInt, false } } },
			{ "max", 0, { HLSVType::UInt, HLSVType::UInt } },
			{ "max", 0, { HLSVType::Float, { HLSVType::Float, false } } },
			{ "max", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "clamp", {
			{ "clamp", 0, { HLSVType::Int, HLSVType::Int, HLSVType::Int } },
			{ "clamp", 0, { HLSVType::Int, { HLSVType::Int, false }, { HLSVType::Int, false } } },
			{ "clamp", 0, { HLSVType::UInt, HLSVType::UInt, HLSVType::UInt } },
			{ "clamp", 0, { HLSVType::UInt, { HLSVType::UInt, false }, { HLSVType::UInt, false } } },
			{ "clamp", 0, { HLSVType::Float, HLSVType::Float, HLSVType::Float } },
			{ "clamp", 0, { HLSVType::Float, { HLSVType::Float, false }, { HLSVType::Float, false } } }
		}},
		{ "mix", {
			{ "mix", 0, { HLSVType::Float, HLSVType::Float, HLSVType::Float } },
			{ "mix", 0, { HLSVType::Float, HLSVType::Float, { HLSVType::Float, false } } }
		}},
		{ "select", {
			{ "mix", 0, { HLSVType::Int, HLSVType::Int, HLSVType::Bool } },
			{ "mix", 0, { HLSVType::UInt, HLSVType::UInt, HLSVType::Bool } },
			{ "mix", 0, { HLSVType::Float, HLSVType::Float, HLSVType::Bool } },
			{ "mix", 0, { HLSVType::Bool, HLSVType::Bool, HLSVType::Bool } }
		}},
		{ "step", {
			{ "step", 0, { HLSVType::Float, HLSVType::Float } },
			{ "step", 1, { { HLSVType::Float, false }, HLSVType::Float } }
		}},
		{ "sstep", {
			{ "smoothstep", 0, { HLSVType::Float, HLSVType::Float, HLSVType::Float } },
			{ "smoothstep", 2, { { HLSVType::Float, false }, { HLSVType::Float, false }, HLSVType::Float } }
		}},
		{ "isnan", {
			{ "isnan", HLSVType::Bool, { { HLSVType::Float, false, true } } },
			{ "isnan", HLSVType::Bool2, { { HLSVType::Float2, false, true } } },
			{ "isnan", HLSVType::Bool3, { { HLSVType::Float3, false, true } } },
			{ "isnan", HLSVType::Bool4, { { HLSVType::Float4, false, true } } }
		}},
		{ "isinf", {
			{ "isinf", HLSVType::Bool, { { HLSVType::Float, false, true } } },
			{ "isinf", HLSVType::Bool2, { { HLSVType::Float2, false, true } } },
			{ "isinf", HLSVType::Bool3, { { HLSVType::Float3, false, true } } },
			{ "isinf", HLSVType::Bool4, { { HLSVType::Float4, false, true } } }
		}},
		{ "ldexp", {
			{ "ldexp", 0, { HLSVType::Float, { HLSVType::Int, true, true } } }
		}},
		// Geometric functions
		{ "len", {
			{ "length", HLSVType::Float, { HLSVType::Float } }
		}},
		{ "dist", {
			{ "distance", HLSVType::Float, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "dot", {
			{ "dot", HLSVType::Float, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "cross", {
			{ "cross", HLSVType::Float3, { HLSVType::Float3, HLSVType::Float3 } }
		}},
		{ "norm", {
			{ "normalize", 0, { HLSVType::Float } }
		}},
		{ "forward", {
			{ "faceForward", 0, { HLSVType::Float, HLSVType::Float, HLSVType::Float } }
		}},
		{ "reflect", {
			{ "reflect", 0, { HLSVType::Float, HLSVType::Float } }
		}},
		{ "refract", {
			{ "refract", 0, { HLSVType::Float, HLSVType::Float, { HLSVType::Float, false } } }
		}},
		// Matrix functions
		{ "matCompMul", {
			{ "matrixCompMult", HLSVType::Mat2, { HLSVType::Mat2, HLSVType::Mat2 } },
			{ "matrixCompMult", HLSVType::Mat3, { HLSVType::Mat3, HLSVType::Mat3 } },
			{ "matrixCompMult", HLSVType::Mat4, { HLSVType::Mat4, HLSVType::Mat4 } }
		}},
		{ "outerProd", {
			{ "outerProduct", HLSVType::Mat2, { HLSVType::Float2, HLSVType::Float2 } },
			{ "outerProduct", HLSVType::Mat3, { HLSVType::Float3, HLSVType::Float3 } },
			{ "outerProduct", HLSVType::Mat4, { HLSVType::Float4, HLSVType::Float4 } }
		}},
		{ "trans", {
			{ "transpose", HLSVType::Mat2, { HLSVType::Mat2 } },
			{ "transpose", HLSVType::Mat3, { HLSVType::Mat3 } },
			{ "transpose", HLSVType::Mat4, { HLSVType::Mat4 } }
		}},
		{ "det", {
			{ "determinant", HLSVType::Float, { HLSVType::Mat2 } },
			{ "determinant", HLSVType::Float, { HLSVType::Mat3 } },
			{ "determinant", HLSVType::Float, { HLSVType::Mat4 } }
		}},
		{ "inv", {
			{ "inverse", HLSVType::Mat2, { HLSVType::Mat2 } },
			{ "inverse", HLSVType::Mat3, { HLSVType::Mat3 } },
			{ "inverse", HLSVType::Mat4, { HLSVType::Mat4 } }
		}},
		// Vector relational functions
		{ "vecLT", {
			{ "lessThan", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "lessThan", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "lessThan", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool }
		}},
		{ "vecLE", {
			{ "lessThanEqual", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "lessThanEqual", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "lessThanEqual", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool }
		}},
		{ "vecGT", {
			{ "greaterThan", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "greaterThan", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "greaterThan", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool }
		}},
		{ "vecGE", {
			{ "greaterThanEqual", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "greaterThanEqual", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "greaterThanEqual", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool }
		}},
		{ "vecEQ", {
			{ "equal", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "equal", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "equal", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool },
			{ "equal", 0, { HLSVType::Bool, HLSVType::Bool }, HLSVType::Bool }
		}},
		{ "vecNE", {
			{ "notEqual", 0, { HLSVType::Int, HLSVType::Int }, HLSVType::Bool },
			{ "notEqual", 0, { HLSVType::UInt, HLSVType::UInt }, HLSVType::Bool },
			{ "notEqual", 0, { HLSVType::Float, HLSVType::Float }, HLSVType::Bool },
			{ "notEqual", 0, { HLSVType::Bool, HLSVType::Bool }, HLSVType::Bool }
		}},
		{ "any", {
			{ "any", HLSVType::Bool, { HLSVType::Bool } }
		}},
		{ "all", {
			{ "all", HLSVType::Bool, { HLSVType::Bool } }
		}},
		{ "not", {
			{ "not", 0, { HLSVType::Bool } }
		}},
		// Texture info
		{ "sizeof", {
			{ "textureSize", HLSVType::Int, { HLSVType::Tex1D, { HLSVType::Int, false } } },
			{ "textureSize", HLSVType::Int2, { HLSVType::Tex2D, { HLSVType::Int, false } } },
			{ "textureSize", HLSVType::Int3, { HLSVType::Tex3D, { HLSVType::Int, false } } },
			{ "textureSize", HLSVType::Int2, { HLSVType::TexCube, { HLSVType::Int, false } } },
			{ "textureSize", HLSVType::Int2, { HLSVType::Tex1DArray, { HLSVType::Int, false } } },
			{ "textureSize", HLSVType::Int3, { HLSVType::Tex2DArray, { HLSVType::Int, false } } },
			{ "imageSize", HLSVType::Int, { HLSVType::Image1D } },
			{ "imageSize", HLSVType::Int2, { HLSVType::Image2D } },
			{ "imageSize", HLSVType::Int3, { HLSVType::Image3D } },
			{ "imageSize", HLSVType::Int2, { HLSVType::Image1DArray } },
			{ "imageSize", HLSVType::Int3, { HLSVType::Image2DArray } }
		}},
		{ "levelsof", {
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::Tex1D } },
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::Tex2D } },
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::Tex3D } },
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::TexCube } },
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::Tex1DArray } },
			{ "textureQueryLevels", HLSVType::Int, { HLSVType::Tex2DArray } }
		}},
		// Texture/image lookups and stores
		{ "load", {
			{ "texture", HLSVType::Float4, { HLSVType::Tex1D, { HLSVType::Float, false } } }, // Normal texture lookups
			{ "texture", HLSVType::Float4, { HLSVType::Tex2D, HLSVType::Float2 } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex3D, HLSVType::Float3 } },
			{ "texture", HLSVType::Float4, { HLSVType::TexCube, HLSVType::Float3 } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex1DArray, HLSVType::Float2 } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex2DArray, HLSVType::Float3 } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex1D, { HLSVType::Float, false }, { HLSVType::Float, false } } }, // Biased texture lookups
			{ "texture", HLSVType::Float4, { HLSVType::Tex2D, HLSVType::Float2, { HLSVType::Float, false } } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex3D, HLSVType::Float3, { HLSVType::Float, false } } },
			{ "texture", HLSVType::Float4, { HLSVType::TexCube, HLSVType::Float3, { HLSVType::Float, false } } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex1DArray, HLSVType::Float2, { HLSVType::Float, false } } },
			{ "texture", HLSVType::Float4, { HLSVType::Tex2DArray, HLSVType::Float3, { HLSVType::Float, false } } },
			{ "imageLoad", 0, { HLSVType::Image1D, { HLSVType::Int, false } } },
			{ "imageLoad", 0, { HLSVType::Image2D, { HLSVType::Int2, false } } },
			{ "imageLoad", 0, { HLSVType::Image3D, { HLSVType::Int3, false } } },
			{ "imageLoad", 0, { HLSVType::Image1DArray, { HLSVType::Int2, false } } },
			{ "imageLoad", 0, { HLSVType::Image2DArray, { HLSVType::Int3, false } } },
			{ "subpassLoad", HLSVType::Float4, { HLSVType::SubpassInput } }
		}},
		{ "loadLod", {
			{ "textureLod", HLSVType::Float4, { HLSVType::Tex1D, { HLSVType::Float, false }, { HLSVType::Float, false } } },
			{ "textureLod", HLSVType::Float4, { HLSVType::Tex2D, HLSVType::Float2, { HLSVType::Float, false } } },
			{ "textureLod", HLSVType::Float4, { HLSVType::Tex3D, HLSVType::Float3, { HLSVType::Float, false } } },
			{ "textureLod", HLSVType::Float4, { HLSVType::TexCube, HLSVType::Float3, { HLSVType::Float, false } } },
			{ "textureLod", HLSVType::Float4, { HLSVType::Tex1DArray, HLSVType::Float2, { HLSVType::Float, false } } },
			{ "textureLod", HLSVType::Float4, { HLSVType::Tex2DArray, HLSVType::Float3, { HLSVType::Float, false } } }
		}},
		{ "fetch", { // Cannot fetch on TexCube per GLSL spec
			{ "texelFetch", HLSVType::Float4, { HLSVType::Tex1D, { HLSVType::Int, false }, { HLSVType::Int, false } } },
			{ "texelFetch", HLSVType::Float4, { HLSVType::Tex2D, HLSVType::Int2, { HLSVType::Int, false } } },
			{ "texelFetch", HLSVType::Float4, { HLSVType::Tex3D, HLSVType::Int3, { HLSVType::Int, false } } },
			{ "texelFetch", HLSVType::Float4, { HLSVType::Tex1DArray, HLSVType::Int2, { HLSVType::Int, false } } },
			{ "texelFetch", HLSVType::Float4, { HLSVType::Tex2DArray, HLSVType::Int3, { HLSVType::Int, false } } }
		}},
		{ "store", {
			IM_STR(Image1D, Int, Int), IM_STR(Image1D, Int2, Int), IM_STR(Image1D, Int4, Int),
			IM_STR(Image1D, UInt, Int), IM_STR(Image1D, UInt2, Int), IM_STR(Image1D, UInt4, Int),
			IM_STR(Image1D, Float, Int), IM_STR(Image1D, Float2, Int), IM_STR(Image1D, Float4, Int),

			IM_STR(Image2D, Int, Int2), IM_STR(Image2D, Int2, Int2), IM_STR(Image2D, Int4, Int2),
			IM_STR(Image2D, UInt, Int2), IM_STR(Image2D, UInt2, Int2), IM_STR(Image2D, UInt4, Int2),
			IM_STR(Image2D, Float, Int2), IM_STR(Image2D, Float2, Int2), IM_STR(Image2D, Float4, Int2),

			IM_STR(Image3D, Int, Int3), IM_STR(Image3D, Int2, Int3), IM_STR(Image3D, Int4, Int3),
			IM_STR(Image3D, UInt, Int3), IM_STR(Image3D, UInt2, Int3), IM_STR(Image3D, UInt4, Int3),
			IM_STR(Image3D, Float, Int3), IM_STR(Image3D, Float2, Int3), IM_STR(Image3D, Float4, Int3),

			IM_STR(Image1DArray, Int, Int2), IM_STR(Image1DArray, Int2, Int2), IM_STR(Image1DArray, Int4, Int2),
			IM_STR(Image1DArray, UInt, Int2), IM_STR(Image1DArray, UInt2, Int2), IM_STR(Image1DArray, UInt4, Int2),
			IM_STR(Image1DArray, Float, Int2), IM_STR(Image1DArray, Float2, Int2), IM_STR(Image1DArray, Float4, Int2),

			IM_STR(Image2DArray, Int, Int3), IM_STR(Image2DArray, Int2, Int3), IM_STR(Image2DArray, Int4, Int3),
			IM_STR(Image2DArray, UInt, Int3), IM_STR(Image2DArray, UInt2, Int3), IM_STR(Image2DArray, UInt4, Int3),
			IM_STR(Image2DArray, Float, Int3), IM_STR(Image2DArray, Float2, Int3), IM_STR(Image2DArray, Float4, Int3)
		}}
	});

	Populated_ = true;
}

} // namespace hlsv
