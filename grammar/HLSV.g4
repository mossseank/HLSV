// This is the parser grammar for the HLSV shader language, used by Antlr to generate a parser for
//    the language definition.
// This file is licensed under the MIT license.
// Copyright (c) Sean Moss 2019

parser grammar HLSV;
options {
    tokenVocab=HLSVLexer;
}

// Top-level file unit
file
    : shaderVersionStatement topLevelStatement* EOF
    ;

// Shader version/type statement
shaderVersionStatement
    : 'shader' VERSION_LITERAL ('graphics' | 'compute') ';'
    ;

// All top-level statements that can appear in the root scope of the source
topLevelStatement
    : vertexAttributeStatement
    | fragmentOutputStatement
    | localStatement
    | uniformStatement
    | pushConstantsStatement
    | constantStatement
    | stageFunction
    ;

// Vertex attribute statement
vertexAttributeStatement
    : 'attr' '(' Index=INTEGER_LITERAL ')' variableDeclaration ';'
    ;

// Fragment output statement
fragmentOutputStatement
    : 'frag' '(' Index=INTEGER_LITERAL ')' variableDeclaration ';'
    ;

// Local variable statement
localStatement
    : 'local' 'flat'? variableDeclaration ';'
    ;

// Uniforms and uniform blocks
uniformStatement
    : 'unif' '(' (Set=INTEGER_LITERAL ',')? Binding=INTEGER_LITERAL ')' 
        (
            variableDeclaration | 
            ('block' variableBlock)
        ) ';'
    ;

// Push constants
pushConstantsStatement
    : 'push' 'block' variableBlock ';'
    ;

// Constants and specialization constants
constantStatement
    : 'const' ('(' Index=INTEGER_LITERAL ')')? variableDeclaration '=' Value=atom ';'
    ;

// Stage functions
stageFunction
    : '@vert' block     # vertFunction
    | '@frag' block     # fragFunction
    ;


// Statements (any line that can stand alone in meaning within a function body)
block
    : '{' statement* '}'
    ;
statement
    : variableDefinition ';'
    | variableDeclaration ';'
    | assignment ';'
    | ifStatement
    | whileLoop
    | doLoop
    | forLoop
    ;

// Variable declaration
variableDeclaration
    : Type=IDENTIFIER typeArgument? Name=IDENTIFIER ('[' Size=INTEGER_LITERAL ']')?
    ;
typeArgument
    : '<' (Format=IDENTIFIER | Index=INTEGER_LITERAL) '>'
    ;
variableBlock
    : '{' (Declarations+=variableDeclaration ';')* '}'
    ;
variableDefinition
    : variableDeclaration '=' Value=expression
    ;

// Assignments
assignment
    : LVal=lvalue Op=('='|'+='|'-='|'*='|'/='|'%='|'<<='|'>>='|'&='|'|='|'^=') Value=expression
    ;
lvalue
    : Name=IDENTIFIER
    | LVal=lvalue '.' SWIZZLE
    | LVal=lvalue '[' Index=expression ']'
    ;

// If statement
ifStatement
    : 'if' '(' Cond=expression ')' (statement|block) (Elifs+=elifStatement)* Else=elseStatement?
    ;
elifStatement
    : 'elif' '(' Cond=expression ')' (statement|block)
    ;
elseStatement
    : 'else' (statement|block)
    ;

// While/do-while
whileLoop
    : 'while' '(' Cond=expression ')' (statement|block)
    ;
doLoop
    : 'do' (statement|block) 'while' '(' Cond=expression ')' ';'
    ;

// For loop
forLoop
    : 'for' '(' Init=variableDefinition ';' Cond=expression ';' Update=assignment ')'
        (statement|block)
    ;


// Expressions (anything that can evaluate to a typed value) (enforce order of operation)
// See https://www.khronos.org/files/opengl45-quick-reference-card.pdf (page 9) for GLSL Order of Operations
expression
    : atom  # AtomExpr
    // Unary operators
    | Expr=IDENTIFIER Op=('--'|'++')    # PostfixExpr
    | Op=('--'|'++') Expr=IDENTIFIER    # PrefixExpr
    | Op=('+'|'-') Expr=expression      # FactorExpr
    | Op=('!'|'~') Expr=expression      # NegateExpr
    // Binary Operators
    | Left=expression Op=('*'|'/'|'%') Right=expression         # MulDivModExpr
    | Left=expression Op=('+'|'-') Right=expression             # AddSubExpr
    | Left=expression Op=('<<'|'>>') Right=expression           # BitShiftExpr
    | Left=expression Op=('<'|'>'|'<='|'>=') Right=expression   # RelationalExpr
    | Left=expression Op=('=='|'!=') Right=expression           # EqualityExpr
    | Left=expression Op=('&'|'|'|'^') Right=expression         # BitLogicExpr
    | Left=expression Op=('&&'|'||') Right=expression           # BoolLogicExpr
    // Ternary (selection) operator
    | Cond=expression '?' TExpr=expression ':' FExpr=expression     # TernaryExpr
    ;

// Atomic expressions (those that cannot be subdivided)
atom
    : '(' expression ')'                # ParenAtom
    | atom '[' Index=expression ']'     # ArrayIndexerAtom
    | atom '.' SWIZZLE                  # SwizzleAtom
    | initializerList                   # InitListAtom
    | functionCall                      # FunctionCallAtom
    | scalarLiteral                     # LiteralAtom
    | IDENTIFIER                        # VariableAtom
    ;
initializerList
    : '{' Args+=expression (',' Args+=expression)* '}'
    ;
functionCall // Includes function calls, type constructions, and casting
    : Name=IDENTIFIER '(' Args+=expression (',' Args+=expression)* ')'
    ;
scalarLiteral
    : INTEGER_LITERAL
    | FLOAT_LITERAL
    | BOOLEAN_LITERAL
    ;
