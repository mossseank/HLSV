// This is the lexer grammar for the HLSV shader language, used by Antlr to generate a lexer for
//    the language definition.
// This file is licensed under the MIT license.
// Copyright (c) Sean Moss 2019

lexer grammar HLSVLexer;

// Keywords
KW_ATTR         : 'attr' ;
KW_COMPUTE      : 'compute' ;
KW_FLAT         : 'flat' ;
KW_FRAG         : 'frag' ;
KW_GRAPHICS     : 'graphics' ;
KW_LOCAL        : 'local' ;
KW_SHADER       : 'shader' ;
KW_UNIF         : 'unif' ;

// Literals
VERSION_LITERAL
    : DigitChar DigitChar DigitChar
    ;

// Numeric literals
SIZE_LITERAL // Special type of decimal, non-negative integer
    : DecimalLiteral
    ;
fragment DecimalLiteral : DigitChar+ ;

// Identifiers, valid for variable and type names, also includes the built-in variables
IDENTIFIER
    : (AlphaChar|'_') (AlnumChar|'_')*
    | '$' AlphaChar+
    ;

// Punctuators
LBRACKET    : '[' ;
RBRACKET    : ']' ;
LPAREN      : '(' ;
RPAREN      : ')' ;
SEMI_COLON  : ';' ;
COMMA       : ',' ;

// Whitespace and comments (ignore)
WS
    : [ \t\r\n\u000C]+ -> channel(HIDDEN)
    ;
COMMENT
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;
LINECOMMENT
    : '//' ~[\r\n]* (('\r'? '\n') | EOF) -> channel(HIDDEN)
    ;

// Character Types
fragment AlphaChar      : [a-zA-Z] ;
fragment DigitChar      : [0-9] ;
fragment AlnumChar      : AlphaChar | DigitChar ;
fragment HexDigitChar   : [a-fA-F0-9] ;
