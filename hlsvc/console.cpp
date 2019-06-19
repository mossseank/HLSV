/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements console.hpp.

#include "console.hpp"
#include <iostream>
#include <cstdarg>


static const std::string YELLOW_TAG = "\x1B[33m";
static const std::string RED_TAG = "\x1B[31m";
static const std::string RESET_TAG = "\x1B[0m";

#define BUF_LEN (512)

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <VersionHelpers.h>
const bool Console::HAS_COLORS = ([]{
	HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(con, &mode);
	SetConsoleMode(con, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	GetConsoleMode(con, &mode);
	return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING);
})();
#else // Unix
const bool Console::HAS_COLORS = true; // Unix supports color escape sequences always
#endif // _WIN32
bool Console::UseIndent_ = false;


// ====================================================================================================================
std::string Console::StrArg(const char* const fmt, ...)
{
	char buf[BUF_LEN];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, BUF_LEN, fmt, args);
	va_end(args);
	return { buf };
}

// ====================================================================================================================
void Console::Info(const std::string& msg)
{
	std::cout << msg << std::endl;
}

// ====================================================================================================================
void Console::Warn(const std::string& msg)
{
	if (HAS_COLORS)
		std::cout << YELLOW_TAG << (UseIndent_ ? "  Warn: " : "Warn: ") << msg << RESET_TAG << std::endl;
	else
		std::cout << (UseIndent_ ? "  Warn: " : "Warn: ") << msg << std::endl;
}

// ====================================================================================================================
void Console::Error(const std::string& msg)
{
	if (HAS_COLORS)
		std::cout << RED_TAG << (UseIndent_ ? "  Error: " : "Error: ") << msg << RESET_TAG << std::endl;
	else
		std::cout << (UseIndent_ ? "  Error: " : "Error: ") << msg << std::endl;
}
