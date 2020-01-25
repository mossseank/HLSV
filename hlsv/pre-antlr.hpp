/* 
 * MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
 * file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

/*
 * Disables warning(s) caused by using ANTLR with C++17.
 */

#pragma once

#include "hlsv-priv.hpp"

#ifdef HLSV_MSVC
#	pragma warning( disable : 4996 )
#endif // HLSV_MSVC