/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements variable.hpp
 */

#include "variable.hpp"


namespace hlsv
{

// ====================================================================================================================
Variable::Variable(const string& name, SVType type, VarScope scope) :
	name{ name },
	gen_name{ name },
	type{ type },
	scope{ scope },
	extra{ },
	read{ GetDefaultRead(scope) },
	write{ GetDefaultWrite(scope) }
{

}

// ====================================================================================================================
Variable::Variable(const string& name, SVType type, VarScope scope, PipelineStages read, PipelineStages write) :
	name{ name },
	gen_name{ name },
	type{ type },
	scope{ scope },
	extra{ },
	read{ read },
	write{ write }
{

}

// ====================================================================================================================
PipelineStages Variable::GetDefaultRead(VarScope scope)
{
	switch (scope) {
		case VarScope::Attribute:    return PipelineStages::Vertex;
		case VarScope::Output:       return PipelineStages::None;
		case VarScope::Local:        return PipelineStages::Fragment;
		case VarScope::Uniform:      return PipelineStages::AllGraphics;
		case VarScope::PushConstant: return PipelineStages::AllGraphics;
		case VarScope::Constant:     return PipelineStages::AllGraphics;
		case VarScope::Block:        return PipelineStages::AllGraphics;
		default:                     return PipelineStages::None;
	}
}

// ====================================================================================================================
PipelineStages Variable::GetDefaultWrite(VarScope scope)
{
	switch (scope) {
		case VarScope::Attribute:    return PipelineStages::None;
		case VarScope::Output:       return PipelineStages::Fragment;
		case VarScope::Local:        return PipelineStages::Vertex;
		case VarScope::Uniform:      return PipelineStages::None;
		case VarScope::PushConstant: return PipelineStages::None;
		case VarScope::Constant:     return PipelineStages::None;
		case VarScope::Block:        return PipelineStages::AllGraphics;
		default:                     return PipelineStages::None;
	}
}

} // namespace hlsv
