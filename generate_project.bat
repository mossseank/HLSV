@rem The HLSV project and all associated files and assets, including this file, are licensed under the MIT license, the
@rem    text of which can be found in the LICENSE file at the root of this project, and is available online at
@rem    (https://opensource.org/licenses/MIT). In the event of redistribution of this code, this header, or the text of
@rem    the license itself, must not be removed from the source files or assets in which they appear.
@rem Copyright (c) 2019 Sean Moss [moss.seank@gmail.com]

@rem This is the Windows script for generating the build solutions for the HLSV project.

@echo off

start /D "./premake" /W /B premake5.exe --file=..\hlsv.project vs2019
