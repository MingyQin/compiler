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
    struct factor *innerFactor;
} unaryOp;

// Basic unit 
// Can be a constant int, expression in parentheses, or a unaryOp
typedef struct {
    enum {
        CONST, PAREN_EXP, UNARY_OP
    } type;
    int value;
    struct expression *exp;
    unaryOp *unOp;
} factor;

// List of factors being * or /
typedef struct {
    factor *factor;
    binaryOp *binOp;
} term;
 
// List of terms being + or -
/*typedef struct expression {
    term *term;
    binaryOp *binOp;
} expression;*/


typedef struct {
    //expression *exp;
} statement;

typedef struct {
    char *id;
    statement *statement;
} function;

typedef struct {
    function *func;
} program;

#endif 