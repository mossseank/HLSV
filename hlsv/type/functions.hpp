/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file declares the function registry, which is used to check function calls against a registry of known and
//    valid function calls, type constructions, and castings

#pragma once

#include "../config.hpp"
#include "../visitor/expr.hpp"
#include <algorithm>
#include <map>


namespace hlsv
{

// Contains a single set of arguments that are valid for a function, and a way to check a given set against them
struct FunctionEntry final
{
public:
	uint32 version;         // The minimum shader version that the function is available in
	string out_name;
	HLSVType return_type;
	std::vector<HLSVType> types;

	FunctionEntry() : 
		version{ 0 }, out_name{ "" }, return_type{ HLSVType::Error }, types{ }
	{ }
	FunctionEntry(uint32 v, const string& name, HLSVType rt, const std::initializer_list<HLSVType>& typ) :
		version{ v }, out_name{ name }, return_type{ rt }, types{ typ }
	{ }
	FunctionEntry(uint32 v, const string& name, HLSVType rt, const std::initializer_list<HLSVType::PrimType>& typ) :
		version{ v }, out_name{ name }, return_type{ rt }, types{ typ.begin(), typ.end() }
	{ }

	bool matches(const std::vector<HLSVType>& args) const;
	inline bool matches(const std::vector<Expr*>& args) const {
		std::vector<HLSVType> atyp{ args.size(), HLSVType::Error };
		std::transform(args.begin(), args.end(), atyp.data(), [](Expr* e) { return e->type; });
		return matches(atyp);
	}
}; // struct FunctionEntry

class FunctionRegistry final
{
private:
	static bool Populated_;
	static std::map<string, std::vector<FunctionEntry>> Functions_;

public:
	static bool CheckFunction(const string& name, const std::vector<HLSVType>& args, string& err, HLSVType& ret, string& outname);
	static bool CheckFunction(const string& name, const std::vector<Expr*>& args, string& err, HLSVType& ret, string& outname);
	static bool CheckConstructor(HLSVType::PrimType type, const std::vector<HLSVType>& args, string& err);
	inline static bool CheckConstructor(HLSVType::PrimType type, const std::vector<Expr*>& args, string& err) {
		std::vector<HLSVType> atyp{ args.size(), HLSVType::Error };
		std::transform(args.begin(), args.end(), atyp.data(), [](Expr* e) { return e->type; });
		return CheckConstructor(type, atyp, err);
	}

private:
	static void Populate();
}; // class FunctionRegistry

} // namespace hlsv
