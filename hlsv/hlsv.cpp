/*
 * The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
 *    text of which can be found in the LICENSE file at the root of this project, and is available online at
 *    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
 *    the license itself, must not be removed from the source files or assets in which they appear.
 * Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]
 */

// This file implements the support (non-API) code found in the public API and config.hpp files.

#include "config.hpp"
#include <cstdarg>

#define STRARG_BUF_SIZE (512)


namespace hlsv
{

// ====================================================================================================================
string strarg(const char* const fmt, ...)
{
	char buf[STRARG_BUF_SIZE];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, STRARG_BUF_SIZE, fmt, args);
	va_end(args);
	return { buf };
}

} // namespace hlsv
