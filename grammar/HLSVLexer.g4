// This is the lexer grammar for the HLSV shader language, used by Antlr to generate a lexer for
//    the language definition.
// This file is licensed under the MIT license.
// Copyright (c) Sean Moss 2019

lexer grammar HLSVLexer;

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
