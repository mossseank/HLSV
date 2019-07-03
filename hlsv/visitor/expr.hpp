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
	} literal_value;

public:
	Expr() : Expr(HLSVType::Error) { }
	explicit Expr(HLSVType type) :
		type{ type }, is_literal{ false }, is_compile_constant{ false }, literal_value{ 0ull }
	{ }
}; // class Expr

} // namespace hlsv
