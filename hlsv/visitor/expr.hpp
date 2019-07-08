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
		bool b;
		double f;
		int64 i;
	} default_value; // This must exactly match the "default_value" union in the SpecConstant type
	string init_text; // The text used to initialize the expression value
	string ref_text;  // The text used to refer to the expression value (for SSA)

public:
	Expr() : Expr(HLSVType::Error) { }
	explicit Expr(HLSVType type) :
		type{ type }, is_literal{ false }, is_compile_constant{ false }, default_value{ 0ull },
		init_text{ "" }, ref_text{ "" }
	{ }
	Expr(const Expr& o) :
		type{ o.type }, is_literal{ o.is_literal }, is_compile_constant{ o.is_compile_constant },
		default_value{ o.default_value }, init_text{ o.init_text }, ref_text{ o.ref_text }
	{ }

	inline void set_default_value(bool b) {
		is_literal = true; default_value.b = b; ref_text = init_text = b ? "true" : "false";
	}
	inline void set_default_value(double f) {
		is_literal = true; default_value.f = f; ref_text = init_text = strarg("%f", f);
	}
	inline void set_default_value(int64 i) {
		is_literal = true; default_value.i = i; ref_text = init_text = strarg("%lld", i);
	}
}; // class Expr

// This enforces keeping the default_value unions of the different types equivalent to the first order
static_assert(sizeof(Expr::default_value) == sizeof(SpecConstant::default_value),
	"The sizes of the `default_value` unions between SpecConstant and Expr do not match.");

} // namespace hlsv
