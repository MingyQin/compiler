#ifndef TYPES_H
#define TYPES_H

typedef enum {
    // Keywords
    RETURN, KEYWORD_INT, 
    
    // Identifier
    IDENTIFIER,

    // Can be subtraction or negation
    MINUS,

    // Unary ops
    BITWISE_COMP, LOGICAL_NEG,

    // Math
    MULTIPLY, ADD, DIVIDE,

    // Datatypes
    INT,

    // Comparison operators (2 character)
    OR, AND, EQUALS, NOT_EQUAL, LESS_EQUAL, GREATER_EQUAL,

    // Comparison operators (1 character)
    GREATER, LESS, 


    // Single-character (punctuation)
    OPEN_BRACE, CLOSED_BRACE, OPEN_PAR, CLOSED_PAR, SEMICOLON
} token_type;

typedef struct {
    int type; // Uses types from keywords.h
    char *lexeme;
    int value;
} token;


typedef struct {
    token *list;
    int numTokens;
    int maxSize;
    int nextToken;
} token_list;


// Structs for the AST

struct expression;

typedef struct {
    char operator;
    struct expression *innerExp;
} unaryOp;

typedef struct {
    token_type operator; // Should only be binary operators and not keywords of grammar
    struct expression *expL;
    struct expression *expR; 
} binaryOp;

// Only supports integer variables for now
typedef struct variable {
    int value;
    char *id; 
} variable;
 
typedef struct expression {
    int type; //Uses types from keywords.h
    int value;
    unaryOp *unOp;
    binaryOp *binOp;
    variable *var;
} expression;

typedef struct {
    int type; // Uses types from the keywords.h
    expression *exp;
} statement;

typedef struct {
    char *id;
    statement **statements; //List of statements
    int numStatements; // Length of statement list
} function;

typedef struct {
    function *func;
} program;

#endif 