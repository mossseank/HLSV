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


namespace hlsv
{

// ====================================================================================================================
GLSLGenerator::GLSLGenerator() :
	vert_vars_{ },
	frag_vars_{ }
{
	vert_vars_ << VERSION_STR << '\n' << VERSION_CMT << HLSV_VERSION << "\n\n";
	frag_vars_ << VERSION_STR << '\n' << VERSION_CMT << HLSV_VERSION << "\n\n";
}

// ====================================================================================================================
GLSLGenerator::~GLSLGenerator()
{

}

// ====================================================================================================================
void GLSLGenerator::emit_attribute(const Attribute& attr)
{
	vert_vars_ << "layout(location = " << (uint32)attr.location << ") in " << TypeHelper::GetGLSLStr(attr.type.type) 
		       << ' ' << attr.name;
	if (attr.type.is_array) {
		vert_vars_ << '[' << (uint32)attr.type.count << ']';
	}
	vert_vars_ << ";\n";
}

// ====================================================================================================================
void GLSLGenerator::emit_output(const Output& output)
{
	frag_vars_ << "layout(location = " << (uint32)output.location << ") out " << TypeHelper::GetGLSLStr(output.type.type)
		       << ' ' << output.name << ";\n";
}

// ====================================================================================================================
void GLSLGenerator::emit_local(const Variable& vrbl, uint32 loc)
{
	string locstr = strarg("layout(location = %u) %s", loc, vrbl.local.is_flat ? "flat " : "");
	string varstr = strarg(" %s %s%s;", TypeHelper::GetGLSLStr(vrbl.type.type).c_str(), vrbl.name.c_str(),
		vrbl.type.is_array ? strarg("[%u]", vrbl.type.count).c_str() : "");

	vert_vars_ << locstr << "out" << varstr << '\n';
	frag_vars_ << locstr << "in" << varstr << '\n';
}

// ====================================================================================================================
void GLSLGenerator::emit_handle_uniform(const Uniform& uni)
{
	string targstr =
		uni.type.is_image_type() ? strarg(", %s", TypeHelper::GetImageFormatStr(uni.type.extra.image_format).c_str()) :
		uni.type == HLSVType::SubpassInput ? strarg(", input_attachment_index = %u", (uint32)uni.type.extra.subpass_input_index) : "";
	string locstr = strarg("layout(set = %u, binding = %u%s)", (uint32)uni.set, (uint32)uni.binding, targstr.c_str());
	string varstr = strarg(" uniform %s %s;", TypeHelper::GetGLSLStr(uni.type.type).c_str(), uni.name.c_str());

	if (uni.type != HLSVType::SubpassInput) { // Error to have subpass inputs specified in any stage except fragment
		vert_vars_ << locstr << varstr << '\n';
	}
	frag_vars_ << locstr << varstr << '\n';
}

} // namespace hlsv
