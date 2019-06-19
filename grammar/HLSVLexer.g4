// This is the lexer grammar for the HLSV shader language, used by Antlr to generate a lexer for
//    the language definition.
// This file is licensed under the MIT license.
// Copyright (c) Sean Moss 2019

lexer grammar HLSVLexer;

// Keywords
KW_COMPUTE      : 'compute' ;
KW_GRAPHICS     : 'graphics' ;
KW_SHADER       : 'shader' ;

// Literals
VERSION_LITERAL
    : DigitChar DigitChar DigitChar
    ;

// Punctuators
SEMI_COLON : ';' ;

// Whitespace and comments (ignore)
WS
    : [ \t\r\n\u000C]+ -> channel(HIDDEN)
    ;
COMMENT
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;
LINECOMMENT
    : '//' ~[\r\n]* '\r'? '\n' -> channel(HIDDEN)
    ;

// Character Types
fragment AlphaChar      : [a-zA-Z] ;
fragment DigitChar      : [0-9] ;
fragment AlnumChar      : AlphaChar | DigitChar ;
fragment HexDigitChar   : [a-fA-F0-9] ;