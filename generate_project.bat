@rem MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
@rem This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
@rem file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.

@rem This is the Windows script for generating the build solutions for the HLSV project.

@echo off

start /D "./premake" /W /B premake5.exe --file=..\hlsv.project vs2019
