/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the configuration for the private implementation code. It includes the public API, and then
//    further includes private-only standard includes, configurations, type forwards, and macro definitions.

#pragma once

#include "hlsv/hlsv.hpp"


/* Stringification */
#define _HLSV_XSTR_(x) #x
#define _HLSV_XSTR(x) _HLSV_XSTR_(x)

/* Current Function Name Macro */
#ifdef HLSV_COMPILER_MSVC
#	define _HLSV_FUNC (__FUNCTION__)
#else 
#	define _HLSV_FUNC (__func__)
#endif // HLSV_COMPILER_MSVC

/* Import/Export Macros */
#if !defined(HLSV_STATIC)
#	if defined(HLSV_COMPILER_MSVC)
#		if defined(_HLSV_BUILD)
#			define _EXPORT __declspec(dllexport)
#		else
#			define _EXPORT __declspec(dllimport)
#		endif // defined(_HLSV_BUILD)
#	else
#		define _EXPORT __attribute__((__visibility__("default")))
#	endif // defined(HLSV_COMPILER_MSVC)
#else
#	define _EXPORT
#endif // !defined(HLSV_STATIC)

/* Type Meta-Control */
// Disables the copy constructor and assignment operator for the type
#define _DECLARE_NOCOPY(className)						\
	public:												\
	className(const className&) = delete;				\
	className& operator = (const className&) = delete;
// Disables the move constructor and assignment operator for the type
#define _DECLARE_NOMOVE(className)						\
	public:												\
	className(const className&&) = delete;				\
	className& operator = (const className&&) = delete;
// Stops the type from being instantiated by disabling the default-constructor, copy and move constructors and
//    assignment operators, and new and delete operators
#define _DECLARE_STATIC(className)						\
	public:												\
	className() = delete;								\
	className(const className&) = delete;				\
	className& operator = (const className&) = delete;	\
	className(const className&&) = delete;				\
	className& operator = (const className&&) = delete;	\
	void* operator new (size_t) = delete;				\
	void* operator new (size_t, void*) = delete;		\
	void* operator new[] (size_t) = delete;				\
	void* operator new[] (size_t, void*) = delete;		\
	void operator delete (void*) = delete;				\
	void operator delete (void*, void*) = delete;		\
	void operator delete[] (void*) = delete;			\
	void operator delete[] (void*, void*) = delete;
