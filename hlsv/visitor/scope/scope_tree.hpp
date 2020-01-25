/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Declares the type for managing the scope tree, which tracks variables and assignments.
 */

#pragma once

#include "../../hlsv-priv.hpp"
#include "variable.hpp"
#include <vector>
#include <set>


namespace hlsv
{

// Represents a variable node within a tree of scopes
class Scope final
{
public:
	using List    = std::vector<std::unique_ptr<Scope>>;
	using VarList = std::vector<std::unique_ptr<Variable>>;

	Scope(Scope* parent);
	~Scope();

	const Variable* find(const string& name, bool parent = true) const;
	bool is_set(const string& name, bool parent = true) const;

	bool add(Variable* var);
	inline void set(const string& name) { setlist_.insert(name); }

	inline void add(Scope* scope) { children_.emplace_back(scope); }

	inline List::iterator begin() { return children_.begin(); }
	inline List::iterator end() { return children_.end(); }
	inline List::const_iterator begin() const { return children_.begin(); }
	inline List::const_iterator end() const { return children_.end(); }

private:
	Scope* parent_;
	List children_;
	VarList variables_;
	std::set<string> setlist_;
}; // class Scope

// Represents a scope tree and other information about a shader program
class ScopeTree final
{
public:
	ScopeTree();
	~ScopeTree();

	void populateBuiltins(PipelineType type);

private:
	Scope::VarList globals_;
}; // class ScopeTree

} // namespace hlsv
