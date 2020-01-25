/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Implements scope_tree.hpp
 */

#include "scope_tree.hpp"
#include <algorithm>


namespace hlsv
{

// ====================================================================================================================
Scope::Scope(Scope* parent) :
	parent_{ parent },
	children_{ },
	variables_{ },
	setlist_{ }
{

}

// ====================================================================================================================
Scope::~Scope()
{
	children_.clear();
	variables_.clear();
}

// ====================================================================================================================
const Variable* Scope::find(const string& name, bool parent) const
{
	for (const auto& ptr : variables_) {
		if (ptr->name == name) {
			return ptr.get();
		}
	}
	return (parent && parent_) ? parent_->find(name, true) : nullptr;
}

// ====================================================================================================================
bool Scope::is_set(const string& name, bool parent) const
{
	for (const auto& set : setlist_) {
		if (set == name) {
			return true;
		}
	}
	return (parent && parent_) ? parent_->is_set(name, true) : false;
}

// ====================================================================================================================
bool Scope::add(Variable* var)
{
	auto dup = find(var->name, true);
	if (dup) {
		return false;
	}
	variables_.emplace_back(var);
	return true;
}


// ====================================================================================================================
ScopeTree::ScopeTree() :
	globals_{ }
{

}

// ====================================================================================================================
ScopeTree::~ScopeTree()
{

}

// ====================================================================================================================
void ScopeTree::populateBuiltins(PipelineType type)
{
	Variable* var{ nullptr };
	
	// emplace_back(name, type, scope, read, write)
	if (type == PipelineType::Graphics) {
		// Vertex
		var = globals_.emplace_back(new Variable{ "$VertexIndex",   SVTypes::Int,    VarScope::Builtin, PipelineStages::Vertex, PipelineStages::None }).get();
		var->gen_name = "gl_VertexIndex";
		var = globals_.emplace_back(new Variable{ "$InstanceIndex", SVTypes::Int,    VarScope::Builtin, PipelineStages::Vertex, PipelineStages::None }).get();
		var->gen_name = "gl_InstanceIndex";
		var = globals_.emplace_back(new Variable{ "$Position",      SVTypes::Float4, VarScope::Builtin, PipelineStages::None,   PipelineStages::Vertex }).get();
		var->gen_name = "gl_Position";
		var = globals_.emplace_back(new Variable{ "$PointSize",     SVTypes::Float,  VarScope::Builtin, PipelineStages::None,   PipelineStages::Vertex }).get();
		var->gen_name = "gl_PointSize";

		// Fragment
		var = globals_.emplace_back(new Variable{ "$FragCoord",     SVTypes::Float4, VarScope::Builtin, PipelineStages::Fragment, PipelineStages::None }).get();
		var->gen_name = "gl_FragCoord";
		var = globals_.emplace_back(new Variable{ "$FrontFacing",   SVTypes::Bool,   VarScope::Builtin, PipelineStages::Fragment, PipelineStages::None }).get();
		var->gen_name = "gl_FrontFacing";
		var = globals_.emplace_back(new Variable{ "$PointCoord",    SVTypes::Float2, VarScope::Builtin, PipelineStages::Fragment, PipelineStages::None }).get();
		var->gen_name = "gl_PointCoord";
		var = globals_.emplace_back(new Variable{ "$FragDepth",     SVTypes::Float,  VarScope::Builtin, PipelineStages::Fragment, PipelineStages::Fragment }).get();
		var->gen_name = "gl_FragDepth";
	}
	else { // PipelineType::Compute

	}
}

} // namespace hlsv
