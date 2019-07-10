/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements glsl_generator.hpp

#include "glsl_generator.hpp"
#include "../type/typehelper.hpp"

#define CSTAGE (*stage_funcs_.at(current_stage_))

static const std::string VERSION_STR = "#version 450";
static const std::string VERSION_CMT = "// Generated with hlsvc version ";
static const std::string EXTENSION_STR = "#extension %s : require\n";
static const std::string EXTENSIONS [] = {
	"GL_EXT_scalar_block_layout"
};
static const std::ios_base::openmode DOM = std::ios_base::out | std::ios_base::ate;


namespace hlsv
{

// ====================================================================================================================
GLSLGenerator::GLSLGenerator() :
	vert_vars_{ },
	frag_vars_{ },
	stage_funcs_{
		{ ShaderStages::Vertex, new sstream{ "// Vertex stage\nvoid vert_main() {\n", DOM } },
		{ ShaderStages::TessControl, new sstream{ "// TessControl stage\nvoid tesc_main() {\n", DOM } },
		{ ShaderStages::TessEval, new sstream{ "// TessEval stage\nvoid tese_main() {\n", DOM } },
		{ ShaderStages::Geometry, new sstream{ "// Geometry stage\nvoid geom_main() {\n", DOM } },
		{ ShaderStages::Fragment, new sstream{ "// Fragment stage\nvoid frag_main() {\n", DOM } }
	},
	indent_str_{ "" },
	current_stage_{ ShaderStages::None }
{
	vert_vars_ << VERSION_CMT << HLSV_VERSION << '\n' << VERSION_STR << '\n';
	frag_vars_ << VERSION_CMT << HLSV_VERSION << '\n' << VERSION_STR << '\n';
	for (const auto& ext : EXTENSIONS) {
		string estr = strarg(EXTENSION_STR.c_str(), ext.c_str());
		vert_vars_ << estr;
		frag_vars_ << estr;
	}
	vert_vars_ << '\n';
	frag_vars_ << '\n';
}

// ====================================================================================================================
GLSLGenerator::~GLSLGenerator()
{
	for (auto pair : stage_funcs_)
		delete pair.second;
	stage_funcs_.clear();
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

// ====================================================================================================================
void GLSLGenerator::emit_uniform_block_header(uint32 s, uint32 b)
{
	string head = strarg("layout(set = %u, binding = %u, scalar) uniform Block_%u_%u {", s, b, s, b);
	vert_vars_ << head << '\n';
	frag_vars_ << head << '\n';
}

// ====================================================================================================================
void GLSLGenerator::emit_block_close()
{
	vert_vars_ << "};\n";
	frag_vars_ << "};\n";
}

// ====================================================================================================================
void GLSLGenerator::emit_value_uniform(const Uniform& uni)
{
	string varstr = strarg("\t%s %s%s;", TypeHelper::GetGLSLStr(uni.type.type).c_str(), uni.name.c_str(),
		(uni.type.is_array ? strarg("[%u]", uni.type.count) : "").c_str());
	string cmtstr = strarg(" // Offset: %u, Size: %u\n", uni.block.offset, uni.block.size);
	vert_vars_ << varstr << cmtstr;
	frag_vars_ << varstr << cmtstr;
}

// ====================================================================================================================
void GLSLGenerator::emit_push_constant_block_header()
{
	static const string HEADER = "layout(push_constant, scalar) uniform PushConstants {\n";
	vert_vars_ << HEADER;
	frag_vars_ << HEADER;
}

// ====================================================================================================================
void GLSLGenerator::emit_push_constant(const PushConstant& pc)
{
	string varstr = strarg("\t%s %s%s;", TypeHelper::GetGLSLStr(pc.type.type).c_str(), pc.name.c_str(),
		(pc.type.is_array ? strarg("[%u]", pc.type.count) : "").c_str());
	string cmtstr = strarg(" // Offset: %u, Size: %u\n", pc.offset, pc.size);
	vert_vars_ << varstr << cmtstr;
	frag_vars_ << varstr << cmtstr;
}

// ====================================================================================================================
void GLSLGenerator::emit_spec_constant(const SpecConstant& sc, const Expr& expr)
{
	string locstr = strarg("layout(constant_id = %u)", (uint32)sc.index);
	string varstr = strarg(" const %s %s = %s;\n", TypeHelper::GetGLSLStr(sc.type.type).c_str(), sc.name.c_str(),
		expr.init_text.c_str());
	vert_vars_ << locstr << varstr;
	frag_vars_ << locstr << varstr;
}

// ====================================================================================================================
void GLSLGenerator::emit_global_constant(const Variable& vrbl, const Expr& expr)
{
	string varstr = strarg("%s %s%s = %s;\n", TypeHelper::GetGLSLStr(vrbl.type.type).c_str(), vrbl.name.c_str(),
		vrbl.type.is_array ? strarg("[%u]", vrbl.type.count).c_str() : "", expr.init_text.c_str());
	vert_vars_ << varstr;
	frag_vars_ << varstr;
}

// ====================================================================================================================
void GLSLGenerator::emit_func_block_close()
{
	CSTAGE << indent_str_ << "}\n";
}

// ====================================================================================================================
void GLSLGenerator::emit_variable_declaration(const Variable& vrbl, Expr* value)
{
	string varstr = TypeHelper::GetGLSLStr(vrbl.type.type) + ' ' + vrbl.name;
	CSTAGE << indent_str_ << varstr;
	if (value)
		CSTAGE << " = " << value->ref_text;
	CSTAGE << ";\n";
}

} // namespace hlsv
