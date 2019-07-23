/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements functions.hpp

#include "functions.hpp"
#include "typehelper.hpp"
#include <stdexcept>


namespace hlsv
{

// ====================================================================================================================
bool FunctionRegistry::Populated_ = false;
std::map<string, std::vector<FunctionEntry>> FunctionRegistry::Functions_{ };

// ====================================================================================================================
bool FunctionEntry::matches(const std::vector<HLSVType>& args) const
{
	if (args.size() != types.size())
		return false;

	// Check the types one at a time
	for (uint32 i = 0; i < args.size(); ++i) {
		auto at = args[i], tt = types[i];
		if (at.is_array != tt.is_array || at.count != tt.count) // Mismatch between array sizes
			return false;
		if (tt.is_array && at.type != tt.type) // Both are arrays, but have different member types
			return false;
		if (!TypeHelper::CanPromoteTo(at.type, tt.type)) // Both are not arrays, but cannot be promoted
			return false;
	}

	return true;
}

// ====================================================================================================================
/* static */
bool FunctionRegistry::CheckFunction(const string& name, const std::vector<HLSVType>& args, string& err, HLSVType& ret, string& outname)
{
	if (!Populated_)
		Populate();

	try {
		const auto& ents = Functions_.at(name);
		for (const auto& e : ents) {
			if (e.matches(args)) {
				ret = e.return_type;
				outname = e.out_name;
				return true;
			}
		}
		err = strarg("No argument list for the function '%s' matches the given arguments.", name.c_str());
		return false;
	}
	catch (std::out_of_range&) {
		err = strarg("The function '%s' does not exist in the current context.", name.c_str());
		return false;
	}
}

// ====================================================================================================================
/* static */
bool FunctionRegistry::CheckFunction(const string& name, const std::vector<Expr*>& args, string& err, HLSVType& ret, string& outname)
{
	if (!Populated_)
		Populate();

	try {
		const auto& ents = Functions_.at(name);
		for (const auto& e : ents) {
			if (e.matches(args)) {
				ret = e.return_type;
				outname = e.out_name;
				return true;
			}
		}
		err = strarg("No argument list for the function '%s' matches the given arguments.", name.c_str());
		return false;
	}
	catch (std::out_of_range&) {
		err = strarg("The function '%s' does not exist in the current context.", name.c_str());
		return false;
	}
}

// ====================================================================================================================
/* static */
bool FunctionRegistry::CheckConstructor(HLSVType::PrimType type, const std::vector<HLSVType>& args, string& err)
{
	if (type == HLSVType::Void) {
		err = "Cannot construct 'void' type.";
		return false;
	}
	if (HLSVType::IsHandleType(type)) {
		err = "Cannot directly construct handle types.";
		return false;
	}

	// Check the arguments
	for (uint32 i = 0; i < args.size(); ++i) {
		if (args[i].is_array) {
			err = strarg("Argument %u of type construction cannot be an array.", i);
			return false;
		}
		if (!args[i].is_value_type()) {
			err = strarg("Argument %u of type construction cannot be a non-value type (%s).", i, args[i].get_type_str().c_str());
			return false;
		}
	}

	// Check each type
	if (HLSVType::IsScalarType(type)) {
		if (args.size() != 1) {
			err = "Scalar casts can only have one argument.";
			return false;
		}
		if (!args[0].is_scalar_type()) {
			err = "Scalar types can only cast from other scalar types.";
			return false;
		}
		return true;
	}
	else if (HLSVType::IsVectorType(type)) {
		auto ctype = HLSVType::GetComponentType(type);
		auto ccount = HLSVType::GetComponentCount(type);
		if (args.size() == 1) { // Direct casting -or- filling with a scalar
			if (args[0].is_scalar_type()) {
				if (!TypeHelper::CanPromoteTo(args[0].type, ctype)) {
					err = strarg("Cannot construct vector type '%s' from scalar type '%s'.", HLSVType::GetTypeStr(type).c_str(),
						args[0].get_type_str().c_str());
					return false;
				}
				return true;
			}
			if (args[0].is_vector_type()) {
				if (args[0].get_component_count() != ccount) {
					err = "Cannot cast between vectors of different sizes.";
					return false;
				}
				if (!TypeHelper::CanPromoteTo(args[0].type, type)) {
					err = strarg("Cannot promote vector type '%s' to vector type '%s'.", args[0].get_type_str().c_str(),
						HLSVType::GetTypeStr(type).c_str());
					return false;
				}
				return true;
			}
			err = "Cannot construct a vector type from a matrix type";
			return false;
		}
		else if (args.size() == ccount) { // Directly specifying each component
			for (uint32 i = 0; i < ccount; ++i) {
				if (!args[i].is_scalar_type() || !TypeHelper::CanPromoteTo(args[i].type, ctype)) {
					err = strarg("'%s' constructor argument %u must be a promotable scalar type.", 
						HLSVType::GetTypeStr(type).c_str(), i);
					return false;
				}
			}
			return true;
		}
		else { // Mixture of types, need to make sure the types match and there are exactly enough components
			auto ctype = HLSVType::GetComponentType(type);
			auto ccount = HLSVType::GetComponentCount(type);
			uint32 acount = 0;
			for (uint32 i = 0; i < args.size(); ++i) {
				if (args[i].is_matrix_type() || !TypeHelper::CanPromoteTo(args[i].get_component_type(), ctype)) {
					err = strarg("'%s' constructor argument %u must be a promotable scalar or vector type.", 
						HLSVType::GetTypeStr(type).c_str(), i);
					return false;
				}
				acount += args[i].get_component_count();
			}
			if (acount != ccount) {
				err = strarg("'%s' constructor expected %u total components, but got %u.", HLSVType::GetTypeStr(type).c_str(),
					(uint32)ccount, acount);
				return false;
			}
			return true;
		}
	}
	else { // Matrix
		auto ctype = HLSVType::GetComponentType(type);
		auto ccount = HLSVType::GetComponentCount(type);
		if (args.size() == 1) {
			if (args[0].is_matrix_type()) return true; // Matrix-matrix casts always work
			if (!args[0].is_scalar_type() || !TypeHelper::CanPromoteTo(args[0].type, ctype)) {
				err = "Diagonal matrices must be constructed from a promotable scalar type.";
				return false;
			}
			return true;
		}
		uint32 acount = 0;
		for (uint32 i = 0; i < args.size(); ++i) {
			if (args[i].is_matrix_type() || !TypeHelper::CanPromoteTo(args[i].get_component_type(), ctype)) {
				err = strarg("Matrix constructor argument %u must be a promotable scalar or vector type.", i);
				return false;
			}
			acount += args[i].get_component_count();
		}
		if (acount != ccount) {
			err = strarg("Matrix constructor expected %u total components, but got %u.", (uint32)ccount, acount);
			return false;
		}
		return true;
	}
}

// ====================================================================================================================
/* static */
void FunctionRegistry::Populate()
{
	Functions_.insert({
		{ "thing", {
			
		}}
	});

	Populated_ = true;
}

} // namespace hlsv
