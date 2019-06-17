#!/usr/bin/env bash 
# This is the grammar build script for Unix.
# This file is public domain.

java -jar antlr-4.7.2-complete.jar \
    -no-listener                   \
    -visitor                       \
    -o ../generated/               \
    -package grammar               \
    -Xexact-output-dir             \
    -Dlanguage=Cpp                 \
    HLSVLexer.g4

java -jar antlr-4.7.2-complete.jar \
    -no-listener                   \
    -visitor                       \
    -o ../generated/               \
    -package grammar               \
    -Xexact-output-dir             \
    -Dlanguage=Cpp                 \
    HLSV.g4