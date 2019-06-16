@rem This is the grammar build script for Windows.
@rem This file is public domain.
@echo off

java -jar antlr-4.7.2-complete.jar ^
    -no-listener                   ^
    -visitor                       ^
    -o ../generated/               ^
    -package grammar               ^
    -Xexact-output-dir             ^
    -Dlanguage=Cpp                 ^
    HLSVLexer.g4

java -jar antlr-4.7.2-complete.jar ^
    -no-listener                   ^
    -visitor                       ^
    -o ../generated/               ^
    -package grammar               ^
    -Xexact-output-dir             ^
    -Dlanguage=Cpp                 ^
    HLSV.g4