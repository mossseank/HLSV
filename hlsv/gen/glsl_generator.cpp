/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements glsl_generator.hpp

#include "glsl_generator.hpp"

static const std::string VERSION_STR = "#version 450";
static const std::string VERSION_CMT = "// Generated with hlsvc version ";


namespace hlsv
{

// ====================================================================================================================
GLSLGenerator::GLSLGenerator() :
	globals_{ }
{
	globals_ << VERSION_STR << "\n";
	globals_ << VERSION_CMT << HLSV_VERSION << "\n";
}

// ====================================================================================================================
GLSLGenerator::~GLSLGenerator()
{

}

} // namespace hlsv
