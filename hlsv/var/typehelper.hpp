/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares helpers and utilities for working with HLSV types

#pragma once

#include "../config.hpp"


namespace hlsv
{

// Performs utilities relating to HLSV types
class TypeHelper final
{
public:
	static HLSVType::PrimType ParseTypeStr(const string& str);
};

} // namespace hlsv
