/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

 // This file implements the registration of builtin funtions to the FunctionRegistry

#include "functions.hpp"


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
		}}
	});

	Populated_ = true;
}

} // namespace hlsv
