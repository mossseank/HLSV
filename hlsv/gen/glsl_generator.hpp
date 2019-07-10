/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the functionality used to generate glsl source code

#pragma once

#include "../config.hpp"
#include "../type/variable.hpp"
#include "../visitor/expr.hpp"
#include <sstream>
#include <map>


namespace hlsv
{

// Generates GLSL source by emitting operations one at a time
class GLSLGenerator final
{
	using sstream = std::ostringstream;

private:
	sstream vert_vars_;
	sstream frag_vars_;
	std::map<ShaderStages, sstream*> stage_funcs_;
	string indent_str_;
	ShaderStages current_stage_;

public:
	GLSLGenerator();
	~GLSLGenerator();

	inline ShaderStages get_stage() const { return current_stage_; }
	inline void set_stage(ShaderStages ss) { current_stage_ = ss; }

	inline string vert_str() const {
		return vert_vars_.str() + '\n' + stage_funcs_.at(ShaderStages::Vertex)->str();
	}
	inline string frag_str() const {
		return frag_vars_.str() + '\n' + stage_funcs_.at(ShaderStages::Fragment)->str();
	}

	void emit_attribute(const Attribute& attr);
	void emit_output(const Output& output);
	void emit_local(const Variable& vrbl, uint32 loc);
	void emit_handle_uniform(const Uniform& uni);
	void emit_uniform_block_header(uint32 s, uint32 b);
	void emit_block_close();
	void emit_value_uniform(const Uniform& uni);
	void emit_push_constant_block_header();
	void emit_push_constant(const PushConstant& pc);
	void emit_spec_constant(const SpecConstant& sc, const Expr& expr);
	void emit_global_constant(const Variable& vrbl, const Expr& expr);

	inline void push_indent() { indent_str_ += '\t'; }
	inline void pop_indent() { indent_str_ = indent_str_.substr(1); }
	void emit_func_block_close();
	void emit_variable_declaration(const Variable& vrbl, Expr* value);
}; // class GLSLGenerator

} // namespace hlsv
