#ifndef TYPES_H
#define TYPES_H

typedef enum {
    // Keywords
    RETURN, KEYWORD_INT, 
    
    // Identifier
    IDENTIFIER,

    // Datatypes
    INT,

    // Single-character (punctuation)
    OPEN_BRACE, CLOSED_BRACE, OPEN_PAR, CLOSED_PAR, SEMICOLON
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    char *lexeme;
    int value;
} token;

// Structs for the AST

typedef struct {
    int value;
} expression;

typedef struct {
    expression *exp;
} statement;

typedef struct {
    char *id;
    statement *statement;
} function;

typedef struct {
    function *func;
} program;

#endif 