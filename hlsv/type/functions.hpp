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

// Information about a function parameter, supports "genType" concept from specification
struct FunctionParam final
{
public:
	HLSVType type;
	bool gen_type;
	bool exact;

	FunctionParam() :
		type{ HLSVType::Error }, gen_type{ false }, exact{ false }
	{ }
	FunctionParam(HLSVType type, bool gt = true, bool exact = false) :
		type{ type }, gen_type{ gt && (type.is_scalar_type() || type.is_image_type()) }, exact{ exact }
	{ }
	FunctionParam(HLSVType::PrimType type, bool gt = true, bool exact = false) :
		type{ type }, gen_type{ gt && (HLSVType::IsScalarType(type) || HLSVType::IsImageType(type)) }, exact{ exact }
	{ }

	bool matches(HLSVType typ) const;
	HLSVType as_return_type(HLSVType rtype, HLSVType atype) const;
};

// Contains a single set of arguments that are valid for a function, and a way to check a given set against them
struct FunctionEntry final
{
public:
	uint32 version;         // The minimum shader version that the function is available in
	string out_name;
	HLSVType return_type;
	std::vector<FunctionParam> params;
	uint32 gen_idx; // Deduces the return type from the gen_type param at this index

	FunctionEntry() : 
		version{ 0 }, out_name{ "" }, return_type{ HLSVType::Error }, params{ }, gen_idx{ UINT32_MAX }
	{ }
	FunctionEntry(const string& name, uint32 genidx, const std::initializer_list<FunctionParam>& pars, uint32 v = 100) :
		version{ v }, out_name{ name }, return_type{ HLSVType::Error }, params{ pars }, gen_idx{ genidx }
	{ }
	FunctionEntry(const string& name, uint32 genidx, const std::initializer_list<FunctionParam>& pars, HLSVType rt, uint32 v = 100) :
		version{ v }, out_name{ name }, return_type{ rt }, params{ pars }, gen_idx{ genidx }
	{ }
	FunctionEntry(const string& name, uint32 genidx, const std::initializer_list<FunctionParam>& pars, HLSVType::PrimType rt, uint32 v = 100) :
		version{ v }, out_name{ name }, return_type{ rt }, params{ pars }, gen_idx{ genidx }
	{ }
	FunctionEntry(const string& name, HLSVType rt, const std::initializer_list<FunctionParam>& pars, uint32 v = 100) :
		version{ v }, out_name{ name }, return_type{ rt }, params{ pars }, gen_idx{ UINT32_MAX }
	{ }
	FunctionEntry(const string& name, HLSVType::PrimType rt, const std::initializer_list<FunctionParam>& pars, uint32 v = 100) :
		version{ v }, out_name{ name }, return_type{ rt }, params{ pars }, gen_idx{ UINT32_MAX }
	{ }

	bool matches(const std::vector<HLSVType>& args, HLSVType& rtype) const;
	inline bool matches(const std::vector<Expr*>& args, HLSVType& rtype) const {
		std::vector<HLSVType> atyp{ args.size(), HLSVType::Error };
		std::transform(args.begin(), args.end(), atyp.data(), [](Expr* e) { return e->type; });
		return matches(atyp, rtype);
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
