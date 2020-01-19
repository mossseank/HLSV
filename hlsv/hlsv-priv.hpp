/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Sets up the private API and utilities.
 */

#pragma once

#include <hlsv/hlsv.hpp>


/* Stringification */
#define _HLSV_XSTR_(x) #x
#define _HLSV_XSTR(x) _HLSV_XSTR_(x)

/* Current Function Name Macro */
#ifdef HLSV_MSVC
#	define _HLSV_FUNC (__FUNCTION__)
#else 
#	define _HLSV_FUNC (__func__)
#endif // HLSV_COMPILER_MSVC

/* Type Meta-Control */
// Disables the copy constructor and assignment operator for the type
#define _HLSV_NOCOPY(className)							\
	public:												\
	className(const className&) = delete;				\
	className& operator = (const className&) = delete;
// Disables the move constructor and assignment operator for the type
#define _HLSV_NOMOVE(className)							\
	public:												\
	className(className&&) = delete;					\
	className& operator = (className&&) = delete;
// Stops the type from being instantiated by disabling the default-constructor, copy and move constructors and
//    assignment operators, and new and delete operators
#define _HLSV_STATIC_CLASS(className)					\
	public:												\
	className() = delete;								\
	className(const className&) = delete;				\
	className& operator = (const className&) = delete;	\
	className(className&&) = delete;					\
	className& operator = (className&&) = delete;		\
	void* operator new (size_t) = delete;				\
	void* operator new (size_t, const std::nothrow_t&) = delete;	\
	void* operator new (size_t, void*) = delete;		\
	void* operator new[] (size_t) = delete;				\
	void* operator new[] (size_t, const std::nothrow_t&) = delete;	\
	void* operator new[] (size_t, void*) = delete;		\
	void operator delete (void*) = delete;				\
	void operator delete (void*, const std::nothrow_t&) = delete;	\
	void operator delete (void*, void*) = delete;		\
	void operator delete[] (void*) = delete;			\
	void operator delete[] (void*, const std::nothrow_t&) = delete;	\
	void operator delete[] (void*, void*) = delete;


namespace hlsv
{

// Performs printf-style formatting of a string.
template<typename... Args>
inline string varstr(const char* const fmt, Args&&... args)
{
	constexpr size_t BUF_SIZE = 512;

	char buf[BUF_SIZE];
	int sz = snprintf(buf, BUF_SIZE, fmt, args...);
	return string(buf, size_t(sz));
}

// Type size checking
static_assert(sizeof(SVType::extra) == 2, "Invalid size for SVType::extra.");

} // namespace hlsv
