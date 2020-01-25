// MIT License - Copyright (c) 2019-2020 Sean Moss [moss.seank@gmail.com]
// This file is subject to the terms and conditions of the MIT License, the text of which can be found in the 'LICENSE'
// file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.

// This is the lexer grammar for the HLSV shader language, used by Antlr to generate a lexer for
//    the language definition.


lexer grammar HLSVLexer;

@header {
    #ifdef _MSC_VER
    #   pragma warning( disable : 4996 ) // Disable deprecation warnings for ANTLR generated code
    #endif // _MSC_VER
}

// Boolean literals
BOOLEAN_LITERAL
    : ('true' | 'false')
    ;

// Keywords
KW_ATTR         : 'attr' ;
KW_BLOCK        : 'block' ;
KW_BREAK        : 'break' ;
KW_COMPUTE      : 'compute' ;
KW_CONST        : 'const' ;
KW_CONTINUE     : 'continue' ;
KW_DISCARD      : 'discard' ;
KW_DO           : 'do' ;
KW_ELIF         : 'elif' ;
KW_ELSE         : 'else' ;
KW_FLAT         : 'flat' ;
KW_FOR          : 'for' ;
KW_FRAG         : 'frag' ;
KW_GRAPHICS     : 'graphics' ;
KW_IF           : 'if' ;
KW_LOCAL        : 'local' ;
KW_PUSH         : 'push' ;
KW_SHADER       : 'shader' ;
KW_UNIF         : 'unif' ;
KW_WHILE        : 'while' ;

// Stage function keywords
KW_STAGE_VERT   : '@vert' ;
KW_STAGE_FRAG   : '@frag' ;

// Swizzles
SWIZZLE
    : SwizzleCharPos+ | SwizzleCharColor+ | SwizzleCharTexCoord+
    ;
fragment SwizzleCharPos         : [xyzw] ;
fragment SwizzleCharColor       : [rgba] ;
fragment SwizzleCharTexCoord    : [stpq] ;

// Literals
VERSION_LITERAL
    : DigitChar DigitChar DigitChar
    ;

// Numeric literals
INTEGER_LITERAL
    : '-'? DecimalLiteral [uU]?
    | '-'? HexLiteral
    | '-'? BinLiteral
    ;
FLOAT_LITERAL
    : '-'? DecimalLiteral '.' DecimalLiteral? ExponentPart?
    ;
fragment DecimalLiteral : DigitChar+ ;
fragment HexLiteral     : ('0x'|'0X') HexDigitChar+ ;
fragment BinLiteral     : ('0b'|'0B') BinDigitChar+ ;
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
COLON       : ':' ;
COMMA       : ',' ;
PERIOD      : '.' ;
Q_MARK      : '?' ;

// Operators
OP_ASSIGN   : '=' ;
OP_ASN_ADD  : '+=' ;
OP_ASN_SUB  : '-=' ;
OP_ASN_MUL  : '*=' ;
OP_ASN_DIV  : '/=' ;
OP_ASN_MOD  : '%=' ;
OP_ASN_LSH  : '<<=' ;
OP_ASN_RSH  : '>>=' ;
OP_ASN_AND  : '&=' ;
OP_ASN_OR   : '|=' ;
OP_ASN_XOR  : '^=' ;
OP_INC      : '++' ;
OP_DEC      : '--' ;
OP_MUL      : '*' ;
OP_DIV      : '/' ;
OP_MOD      : '%' ;
OP_ADD      : '+' ;
OP_SUB      : '-' ;
OP_BANG     : '!' ;
OP_BITNEG   : '~' ;
OP_BITAND   : '&' ;
OP_BITOR    : '|' ;
OP_BITXOR   : '^' ;
OP_AND      : '&&' ;
OP_OR       : '||' ;
OP_LSHIFT   : '<<' ;
OP_RSHIFT   : '>>' ;
OP_LT       : '<' ;
OP_GT       : '>' ;
OP_LE       : '<=' ;
OP_GE       : '>=' ;
OP_EQ       : '==' ;
OP_NE       : '!=' ;

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
fragment BinDigitChar   : [01] ;
