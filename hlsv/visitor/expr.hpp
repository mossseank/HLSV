/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Declares the Expr type, which conveys expression information in the visitor.
 */

#pragma once

#include "../hlsv-priv.hpp"


namespace hlsv
{

// Contains rvalue information from a source tree
class Expr final
{
public:
	SVType type;
	bool literal;
	bool compile_constant; // Value literal or specialization constant
	union
	{
		double d;
		int64 si;
		uint64 ui;
	} literal_value;
	string text; // The generated GLSL text for this expression

	explicit Expr(SVType type) :
		type{ type }, literal{ false }, compile_constant{ false }, literal_value{ 0ull }, text{ "" }
	{ }
	Expr(SVType type, const string& text) :
		type{ type }, literal{ false }, compile_constant{ false }, literal_value{ 0ull }, text{ text }
	{ }

	inline void set_literal(bool b) {
		literal = true; literal_value.ui = b ? 1 : 0; text = b ? "true" : "false";
	}
	inline void set_literal(double d) {
		literal = true; literal_value.d = d; text = varstr("%f", d);
	}
	inline void set_literal(int64 i) {
		literal = true; literal_value.si = i; text = varstr("%lld", i);
	}
	inline void set_literal(uint64 u) {
		literal = true; literal_value.ui = u; text = varstr("%llu", u);
	}
}; // class Expr

} // namespace hlsv
