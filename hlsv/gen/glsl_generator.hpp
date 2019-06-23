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
#include <sstream>


namespace hlsv
{

// Generates GLSL source by emitting operations one at a time
class GLSLGenerator final
{
	using sstream = std::ostringstream;

private:
	sstream globals_; // Holds the uniforms and static values
	sstream attribs_; // Holds the vertex attributes

public:
	GLSLGenerator();
	~GLSLGenerator();

	inline string vert_str() const {
		return globals_.str() + attribs_.str();
	}
	inline string frag_str() const {
		return globals_.str();
	}

	void emit_attribute(const Attribute& attr);
}; // class GLSLGenerator

} // namespace hlsv
