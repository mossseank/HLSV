# HLSV [![Build Status](https://travis-ci.org/mossseank/HLSV.svg?branch=master)](https://travis-ci.org/mossseank/HLSV)
High-Level SPIRV  -  a language designed to simplify writing Vulkan shaders.

HLSV is a C-like GPU shader language very similar to GLSL and HLSL, mainly derived from GLSL for Vulkan. It is designed to be easy to pick up if you know either GLSL or HLSL, and even more so if you are familiar with the GLSL extensions for Vulkan. The language is cross-compiled into Vulkan-style GLSL, which is then compiled into SPIR-V bytecode programs using the existing tools in the Vulkan SDK.

HLSV was created over frustration with some of the limitations of GLSL, its extreme verbosity and error-prone syntax when extended to support Vulkan features, and lack of program reflection. The goal is for it to support all of the common shader functions, and to eventually become a viable alternative to GLSL for most common use cases.

More information, including tutorials and a language overview, can be found on the [project wiki](https://github.com/mossseank/HLSV/wiki).

## Acknowledgements
* [ANTLR](https://www.antlr.org/) - Lexer/parser generator used to read HLSV source
