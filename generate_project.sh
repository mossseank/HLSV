#!/usr/bin/env bash
# The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
#    text of which can be found in the LICENSE file at the root of this project, and is available online at
#    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
#    the license itself, must not be removed from the source files or assets in which they appear.
# Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]

# This is the Unix script for generating the build solutions for the HLSV project.

if [ "$(uname)" = "Darwin" ]; then
	IsMac=1
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
	IsLinux=1
elif [ "$(expr substr $(uname -s) 1 5)" = "MINGW" ]; then
	echo "ERROR: MinGW is not a supported build system for Volu"
	exit 1
elif [ "$(expr substr $(uname -s) 1 6)" = "CYGWIN" ]; then
	echo "ERROR: Cygwin is not a supported build system for Volu"
	exit 1
else
	echo "ERROR: Build platform not understood: $(uname)"
	exit 1
fi

if [ $IsMac -eq 1 ]; then
	./tools/premake5_m --file=./hlsv.project gmake2
elif [ $IsLinux -eq 1 ]; then
	./tools/premake5_l --file=./hlsv.project gmake2
fi
