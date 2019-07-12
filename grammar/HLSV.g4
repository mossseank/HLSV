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


// Expressions (anything that can evaluate to a typed value) (enforce order of operation)
// See https://www.khronos.org/files/opengl45-quick-reference-card.pdf for GLSL Order of Operations
expression
    : atom
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
