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
#include "../var/variable.hpp"
#include <sstream>


namespace hlsv
{

// Generates GLSL source by emitting operations one at a time
class GLSLGenerator final
{
	using sstream = std::ostringstream;

private:
	sstream vert_vars_;
	sstream frag_vars_;

public:
	GLSLGenerator();
	~GLSLGenerator();

	inline string vert_str() const {
		return vert_vars_.str();
	}
	inline string frag_str() const {
		return frag_vars_.str();
	}

	void emit_attribute(const Attribute& attr);
	void emit_output(const Output& output);
	void emit_local(const Variable& vrbl, uint32 loc);
	void emit_handle_uniform(const Uniform& uni);
}; // class GLSLGenerator

} // namespace hlsv
