#ifndef TYPES_H
#define TYPES_H


/*typedef enum {
    // Keywords
    RETURN, KEYWORD_INT, 
    
    // Identifier
    IDENTIFIER,

    // Unary ops
    NEGATION, BITWISE_COMP, LOGICAL_NEG,

    // Datatypes
    INT,

    // Single-character (punctuation)
    OPEN_BRACE, CLOSED_BRACE, OPEN_PAR, CLOSED_PAR, SEMICOLON
} TOKEN_TYPE;*/

typedef struct {
    enum {
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

    // Single-character (punctuation)
    OPEN_BRACE, CLOSED_BRACE, OPEN_PAR, CLOSED_PAR, SEMICOLON
    } type;
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
    //expression *expL;
    //expression *expR;
} binaryOp;

typedef struct {
    char operator;
    struct expression *innerExp;
} unaryOp;
 
typedef struct expression {
    enum {
        BINARY_OP, INTEGER, UNARY_OP
    } type;
    char operator;
    int value;
    unaryOp *unOp;
    struct expression *expL;
    struct expression *expR; 
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