/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements glsl_generator.hpp

#include "glsl_generator.hpp"
#include "../var/typehelper.hpp"

static const std::string VERSION_STR = "#version 450";
static const std::string VERSION_CMT = "// Generated with hlsvc version ";
static const std::string ATTRIB_CMT = "// Vertex attributes";
static const std::string OUTPUT_CMT = "// Fragment outputs";


namespace hlsv
{

// ====================================================================================================================
GLSLGenerator::GLSLGenerator() :
	globals_{ },
	attribs_{ },
	outputs_{ }
{
	globals_ << VERSION_STR << '\n';
	globals_ << VERSION_CMT << HLSV_VERSION << '\n';
	attribs_ << '\n' << ATTRIB_CMT << '\n';
	outputs_ << '\n' << OUTPUT_CMT << '\n';
}

// ====================================================================================================================
GLSLGenerator::~GLSLGenerator()
{

}

// ====================================================================================================================
void GLSLGenerator::emit_attribute(const Attribute& attr)
{
	attribs_ << "layout(location = " << (uint32)attr.location << ") in " << TypeHelper::GetGLSLStr(attr.type.type) << ' '
		     << attr.name;
	if (attr.type.is_array) {
		attribs_ << '[' << (uint32)attr.type.count << ']';
	}
	attribs_ << ";\n";
}

// ====================================================================================================================
void GLSLGenerator::emit_output(const Output& output)
{
	outputs_ << "layout(location = " << (uint32)output.location << ") out " << TypeHelper::GetGLSLStr(output.type.type)
		<< ' ' << output.name << ";\n";
}

} // namespace hlsv
