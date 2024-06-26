#ifndef TYPES_H
#define TYPES_H

typedef enum {
    // Keywords
    RETURN, INT, 
    
    // Identifier
    IDENTIFIER,

    // Single-character (punctuation)
    OPEN_BRACE, CLOSED_BRACE, OPEN_PAR, CLOSED_PAR, SEMI_COLON
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    char *lexeme;
    int value;
} token;
#endif 