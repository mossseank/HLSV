/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the types that read and write shader reflection info to/from the disk.

#pragma once

#include "../config.hpp"


namespace hlsv
{

// The writer
class ReflWriter final
{
public:
	static bool WriteText(const string& path, const ReflectionInfo& refl, string& err);
	static bool WriteBinary(const string& path, const ReflectionInfo& refl, string& err);
}; // class ReflWriter

} // namespace hlsv
