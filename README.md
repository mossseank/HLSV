# HLSV [![Build Status](https://travis-ci.org/mossseank/HLSV.svg?branch=master)](https://travis-ci.org/mossseank/HLSV)
High-Level SPIRV  -  a language designed to simplify writing Vulkan shaders.

HLSV is a C-like GPU shader language very similar to GLSL and HLSL. It is designed to be easy to pick up if you know either GLSL or HLSL, and even more so if you are familiar with the GLSL extensions for Vulkan.

HLSV was created to provide a simpler, less verbose, and less error-prone alternative to GLSL for writing Vulkan shaders. It also provides easy to understand (and parse) reflection info for shader programs.

The language is cross-compiled into Vulkan-style GLSL, which is then compiled into SPIR-V bytecode programs using the existing tools in the Vulkan SDK.

More information, including tutorials and a language overview, can be found on the [project wiki](https://github.com/mossseank/HLSV/wiki).

## Acknowledgements
* [ANTLR](https://www.antlr.org/) - Lexer/parser generator and runtime used to read HLSV source
