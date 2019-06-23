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
static string pad(const string& str, uint32 count)
{
	if (str.size() > count) return str.substr(0, count - 2) + "..";
	return str + string((size_t)count - str.size(), ' ');
}

// ====================================================================================================================
template <typename... Args>
static string padf(const char* const fmt, uint32 count, Args... args)
{
	return pad(strarg(fmt, args...), count);
}

// ====================================================================================================================
static std::ofstream& write_str(std::ofstream& s, const string& str)
{
	s << (uint8)str.length();
	s.write(str.c_str(), (uint8)str.length());
	return s;
}

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
	if (refl.is_graphics()) {
		file << "Stages: ";
		if (refl.stages & ShaderStages::Vertex) file << "Vertex, ";
		if (refl.stages & ShaderStages::TessControl) file << "TessControl, ";
		if (refl.stages & ShaderStages::TessEval) file << "TessEval, ";
		if (refl.stages & ShaderStages::Geometry) file << "Geometry, ";
		if (refl.stages & ShaderStages::Fragment) file << "Fragment";
		file << std::endl;
	}
	file << std::endl;

	// Write vertex attributes
	file << "Attributes" << std::endl 
		 << "----------" << std::endl;
	if (refl.attributes.size() > 0) {
		file << pad("Name", 16) << ' ' << pad("Type", 12) << ' ' << pad("Array", 10) << ' '  << pad("Count", 10) << ' '
			 << pad("Location", 10) << ' ' << pad("Slots", 10) << std::endl;
		for (const auto& attr : refl.attributes) {
			file << pad(attr.name, 16) << ' ' << pad(attr.type.type_str(), 12) << ' ' 
				 << pad(attr.type.is_array ? "Yes" : "No", 10) << ' ' << padf("%u", 10, (uint32)attr.type.count) << ' '
				 << padf("%u", 10, (uint32)attr.location) << ' ' << padf("%u", 10, (uint32)attr.slot_count) << std::endl;
		}
		file << std::endl;
	}
	else
		file << "None" << std::endl << std::endl;
	
	// Close and return
	file.flush();
	file.close();
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
	file << (uint8)(refl.tool_version / 100) << (uint8)((refl.tool_version % 100) / 10) << (uint8)(refl.tool_version % 10);
	file << (uint8)(refl.shader_version / 100) << (uint8)((refl.shader_version % 100) / 10) << (uint8)(refl.shader_version % 10);
	file << (uint8)(refl.is_graphics() ? 0u : 1u);
	file << (uint8)(refl.stages);

	// Write vertex attributes
	file << (uint8)refl.attributes.size();
	if (refl.attributes.size() > 0) {
		for (const auto& attr : refl.attributes) {
			write_str(file, attr.name) << (uint8)attr.type.type << (uint8)attr.type.is_array << attr.type.count
				<< attr.location << attr.slot_count;
		}
	}

	// Close and return
	file.flush();
	file.close();
	return true;
}

} // namespace hlsv
