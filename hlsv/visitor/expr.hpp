/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the Expr type, which is used to represent expressions in the HLSV grammar visitor

#pragma once

#include "../config.hpp"


namespace hlsv
{

// Contains information about an rvalue expresssion in a source tree
class Expr final
{
public:
	HLSVType type;
	bool is_literal;
	bool is_compile_constant; // If it is a compile time constant (value literal or spec. constant ref)
	union
	{
		float f;
		int32 si;
		uint32 ui;
	} literal_value; // This must exactly match the "default_value" union in the SpecConstant type
	string text;  // The text used to refer to the expression value (for SSA)

public:
	Expr() : Expr(HLSVType::Error) { }
	explicit Expr(HLSVType type) :
		type{ type }, is_literal{ false }, is_compile_constant{ false }, literal_value{ 0u },
		text{ "" }
	{ }
	Expr(const Expr& o) :
		type{ o.type }, is_literal{ o.is_literal }, is_compile_constant{ o.is_compile_constant },
		literal_value{ o.literal_value }, text{ o.text }
	{ }

	inline void set_literal_value(bool b) {
		is_literal = true; literal_value.ui = b ? 1u : 0u; text = b ? "true" : "false";
	}
	inline void set_literal_value(float f) {
		is_literal = true; literal_value.f = f; text = strarg("%f", f);
	}
	inline void set_literal_value(int32 i) {
		is_literal = true; literal_value.si = i; text = strarg("%d", i);
	}
	inline void set_literal_value(uint32 i) {
		is_literal = true; literal_value.ui = i; text = strarg("%u", i);
	}
}; // class Expr

// This enforces keeping the default_value unions of the different types equivalent to the first order
static_assert(sizeof(Expr::literal_value) == sizeof(SpecConstant::default_value),
	"Size mismatch between Expr::literal_value and SpecConstant::default_value.");

} // namespace hlsv
