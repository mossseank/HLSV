/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the ReflWriter type in io.hpp

#include "io.hpp"
#include <fstream>


namespace hlsv
{

// ====================================================================================================================
/* static */
bool ReflWriter::WriteText(const string& path, const ReflectionInfo& refl, string& err)
{
	// Open the file for writing
	std::ofstream file{ path, std::ofstream::out | std::ofstream::trunc };
	if (!file.is_open() || !file.good()) {
		err = "invalid path or read-only file";
		return false;
	}

	// Write header
	file << "HLSVC version " << refl.tool_version << ", Shader version " << refl.shader_version << std::endl;
	file << "Shader type " << (refl.is_graphics() ? "GRAPHICS" : "COMPUTE") << std::endl;

	return true;
}

// ====================================================================================================================
/* static */
bool ReflWriter::WriteBinary(const string& path, const ReflectionInfo& refl, string& err)
{
	// Open the file for writing
	std::ofstream file{ path, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary };
	if (!file.is_open() || !file.good()) {
		err = "invalid path or read-only file";
		return false;
	}

	// Write header
	file << 'H' << 'L' << 'S' << 'V';
	file << (uint16)refl.tool_version << (uint16)refl.shader_version;
	file << (uint8)(refl.is_graphics() ? 0u : 1u);

	return true;
}

} // namespace hlsv
