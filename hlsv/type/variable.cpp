/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements variable.hpp

#include "variable.hpp"


namespace hlsv
{

// ====================================================================================================================
Variable::Variable(const string& name, HLSVType type, VarScope scope) :
	name{ name },
	type{ type },
	scope{ scope },
	constant{ false, 0 },
	read{ GetDefaultReadStages(scope) },
	write{ GetDefaultWriteStages(scope) }
{
	
}

// ====================================================================================================================
/* static */
ShaderStages Variable::GetDefaultReadStages(VarScope scope)
{
	switch (scope) {
	case VarScope::Attribute: return ShaderStages::Vertex;
	case VarScope::Output: return ShaderStages::None;
	case VarScope::Local: return ShaderStages::Fragment;
	case VarScope::Uniform: return ShaderStages::AllGraphics;
	case VarScope::PushConstant: return ShaderStages::AllGraphics;
	case VarScope::Constant: return ShaderStages::AllGraphics;
	}
	return ShaderStages::None;
}

// ====================================================================================================================
/* static */
ShaderStages Variable::GetDefaultWriteStages(VarScope scope)
{
	switch (scope) {
	case VarScope::Attribute: return ShaderStages::None;
	case VarScope::Output: return ShaderStages::Fragment;
	case VarScope::Local: return ShaderStages::Vertex;
	case VarScope::Uniform: return ShaderStages::None;
	case VarScope::PushConstant: return ShaderStages::None;
	case VarScope::Constant: return ShaderStages::None;
	}
	return ShaderStages::None;
}

} // namespace hlsv
