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

#define WRITE_LE16(val) uint8((val)&0xFF) << uint8(((val)>>8)&0xFF)


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
		file << pad("Name", 16) << ' ' << pad("Type", 16) << ' ' << pad("Array", 10) << ' '  << pad("Count", 10) << ' '
			 << pad("Location", 10) << ' ' << pad("Slots", 10) << std::endl;
		for (const auto& attr : refl.attributes) {
			file << pad(attr.name, 16) << ' ' << pad(attr.type.get_type_str(), 16) << ' ' 
				 << pad(attr.type.is_array ? "Yes" : "No", 10) << ' ' << padf("%u", 10, (uint32)attr.type.count) << ' '
				 << padf("%u", 10, (uint32)attr.location) << ' ' << padf("%u", 10, (uint32)attr.slot_count) << std::endl;
		}
		file << std::endl;
	}
	else
		file << "None" << std::endl << std::endl;

	// Write fragment outputs
	file << "Outputs" << std::endl
		 << "-------" << std::endl;
	if (refl.outputs.size() > 0) {
		file << pad("Name", 16) << ' ' << pad("Type", 16) << ' ' << pad("Location", 10) << std::endl;
		for (const auto& out : refl.outputs) {
			file << pad(out.name, 16) << ' ' << pad(out.type.get_type_str(), 16) << ' ' << padf("%u", 10, (uint32)out.location)
				 << std::endl;
		}
		file << std::endl;
	}
	else
		file << "None" << std::endl << std::endl;

	// Uniforms
	file << "Uniforms" << std::endl
		 << "--------" << std::endl;
	if (refl.uniforms.size() > 0) {
		file << pad("Name", 16) << ' ' << pad("Type", 16) << ' ' << pad("Type Arg.", 12) << ' ' << pad("Array", 10) << ' '
			 << pad("Set", 8) << ' ' << pad("Binding", 8) << ' ' << pad("Block", 8) << ' ' << pad("Offset", 8) << ' '
			 << pad("Count", 8) << ' ' << pad("Size", 8) << std::endl;
		for (const auto& uni : refl.uniforms) {
			file << pad(uni.name, 16) << ' ' << pad(uni.type.get_type_str(), 16) << ' ';
			if (uni.type.is_image_type()) file << pad(HLSVType::GetTypeStr(uni.type.extra.image_format), 12);
			else if (uni.type == HLSVType::SubpassInput) file << padf("%u", 12, (uint32)uni.type.extra.subpass_input_index);
			else file << pad("", 12);
			file << ' ' << pad(uni.type.is_array ? "Yes" : "No", 10) << ' ' << padf("%u", 8, (uint32)uni.set) << ' '
				 << padf("%u", 8, (uint32)uni.binding);
			if (uni.type.is_value_type()) {
				file << ' ' << padf("%u", 8, (uint32)uni.block.index) << ' ' << padf("%u", 8, (uint32)uni.block.offset)
					 << ' ' << padf("%u", 8, (uint32)uni.type.count) << ' ' << padf("%u", 8, (uint32)uni.block.size)
					 << std::endl;
			}
			else file << std::endl;
		}
		file << std::endl;
	}
	else
		file << "None" << std::endl << std::endl;

	// Uniform blocks
	file << "Uniform Blocks" << std::endl
		 << "--------------" << std::endl;
	if (refl.blocks.size() > 0) {
		file << pad("Set", 8) << ' ' << pad("Binding", 8) << ' ' << pad("Members", 8) << ' ' << pad("Packed", 8) << ' ' 
			 << pad("Size", 8) << std::endl;
		for (const auto& bl : refl.blocks) {
			file << padf("%u", 8, (uint32)bl.set) << ' ' << padf("%u", 8, (uint32)bl.binding) << ' '
				 << padf("%u", 8, (uint32)bl.members.size()) << ' ' << pad(bl.packed ? "Yes" : "No", 8) << ' '
				 << padf("%u", 8, (uint32)bl.size) << std::endl;
		}
		file << std::endl;
	}
	else
		file << "None" << std::endl << std::endl;

	// Push constants
	file << "Push Constants" << std::endl
		 << "--------------" << std::endl;
	if (refl.push_constants.size() > 0) {
		file << pad("Name", 16) << ' ' << pad("Type", 12) << ' ' << pad("Array", 8) << ' ' << pad("Count", 8) << ' '
			 << pad("Offset", 8) << ' ' << pad("Size", 8) << std::endl;
		for (const auto& pc : refl.push_constants) {
			file << pad(pc.name, 16) << ' ' << pad(pc.type.get_type_str(), 12) << ' ' << pad(pc.type.is_array ? "Yes" : "No", 8)
				 << ' ' << padf("%u", 8, (uint32)pc.type.count) << ' ' << padf("%u", 8, (uint32)pc.offset) << ' '
				 << padf("%u", 8, (uint32)pc.size) << std::endl;
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
			write_str(file, attr.name) << (uint8)attr.type.type << (uint8)(attr.type.is_array ? attr.type.count : 0)
				<< attr.location << attr.slot_count;
		}
	}

	// Write fragment outputs
	file << (uint8)refl.outputs.size();
	if (refl.outputs.size() > 0) {
		for (const auto& out : refl.outputs) {
			write_str(file, out.name) << (uint8)out.type.type << (uint8)out.location;
		}
	}

	// Write uniforms
	file << (uint8)refl.uniforms.size();
	if (refl.uniforms.size() > 0) {
		for (const auto& uni : refl.uniforms) {
			write_str(file, uni.name) << (uint8)uni.type.type << (uint8)uni.type.extra.image_format // Writes whatever extra information there is
				<< (uint8)(uni.type.is_array ? uni.type.count : 0) << uni.set << uni.binding << uni.block.index
				<< WRITE_LE16(uni.block.offset) << WRITE_LE16(uni.block.size);
		}
	}

	// Write uniform blocks
	file << (uint8)refl.blocks.size();
	if (refl.blocks.size() > 0) {
		for (const auto& bl : refl.blocks) {
			file << bl.set << bl.binding << WRITE_LE16(bl.size) << (uint8)(bl.packed ? 1 : 0);
			file << (uint8)bl.members.size();
			for (auto mi : bl.members) {
				file << mi;
			}
		}
	}

	// Write push constants
	file << (uint8)refl.push_constants.size();
	if (refl.push_constants.size() > 0) {
		for (const auto& pc : refl.push_constants) {
			write_str(file, pc.name) << (uint8)pc.type.type << (uint8)(pc.type.is_array ? pc.type.count : 0)
				<< WRITE_LE16(pc.offset) << WRITE_LE16(pc.size);
		}
	}

	// Close and return
	file.flush();
	file.close();
	return true;
}

} // namespace hlsv
