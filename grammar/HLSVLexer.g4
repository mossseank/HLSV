// This is the lexer grammar for the HLSV shader language, used by Antlr to generate a lexer for
//    the language definition.
// This file is licensed under the MIT license.
// Copyright (c) Sean Moss 2019

lexer grammar HLSVLexer;

// Boolean literals
BOOLEAN_LITERAL
    : 'true'
    | 'false'
    ;

// Keywords
KW_ATTR         : 'attr' ;
KW_BLOCK        : 'block' ;
KW_COMPUTE      : 'compute' ;
KW_CONST        : 'const' ;
KW_FLAT         : 'flat' ;
KW_FRAG         : 'frag' ;
KW_GRAPHICS     : 'graphics' ;
KW_LOCAL        : 'local' ;
KW_PUSH         : 'push' ;
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
INTEGER_LITERAL
    : '-'? DecimalLiteral [uU]?
    | '-'? HexLiteral [uU]?
    ;
FLOAT_LITERAL
    : '-'? DecimalLiteral '.' DecimalLiteral? ExponentPart?
    ;
fragment DecimalLiteral : DigitChar+ ;
fragment HexLiteral     : '0x' HexDigitChar+ ;
fragment ExponentPart   : [eE] ('-'|'+')? DigitChar+ ;

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
LBRACE      : '{' ;
RBRACE      : '}' ;
SEMI_COLON  : ';' ;
COMMA       : ',' ;

// Operators
OP_LT       : '<' ;
OP_GT       : '>' ;
OP_ADD      : '+' ;
OP_SUB      : '-' ;

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
