#!/usr/bin/env bash
# MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
# This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
# file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.

# This is the Unix script for generating the build solutions for the HLSV project.

IsMac=0
IsLinux=0
if [ "$(uname)" = "Darwin" ]; then
	IsMac=1
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
	IsLinux=1
elif [ "$(expr substr $(uname -s) 1 5)" = "MINGW" ]; then
	echo "ERROR: MinGW is not a supported build system for HLSV"
	exit 1
elif [ "$(expr substr $(uname -s) 1 6)" = "CYGWIN" ]; then
	echo "ERROR: Cygwin is not a supported build system for HLSV"
	exit 1
else
	echo "ERROR: Build platform not understood: $(uname)"
	exit 1
fi

if [ $IsMac -eq 1 ]; then
	./premake/premake5_m --file=./hlsv.project gmake2
elif [ $IsLinux -eq 1 ]; then
	./premake/premake5_l --file=./hlsv.project gmake2
fi
