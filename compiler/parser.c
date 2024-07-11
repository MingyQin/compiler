#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "parser.h"
#include "tokens.h"


program *parseProgram()
{
    program *p = malloc(sizeof(program));
    if (p == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    function *f = parseFunc();
    if (f == NULL)
    {
        free(p);
        return NULL;
    }
    p->func = f;
    return p;
}

// Returns NULL if something went wrong
function *parseFunc()
{
    function *f = malloc(sizeof(function));
    if (f == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    token *tok = nextToken();
    // For now functions can only be of type INT
    if (tok == NULL || tok->type != KEYWORD_INT)
    {
        printf("Missing function return type\n");
        free(f);
        return NULL;
    }
    
    // Look for and set function ID
    tok = nextToken();
    if (tok == NULL || tok->type != IDENTIFIER)
    {
        printf("Missing function name\n");
        free(f);
        return NULL;
    }
    // Point the pointer to the same memory as pointed to by the token's lexeme property
    // This memory is later freed by freeTokens() not freeFunc()
    f->id = tok->lexeme;

    // Parentheses
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_PAR)
    {
        printf("Missing parentheses\n");
        free(f);
        return NULL;

    }
    tok = nextToken();
    if (tok == NULL || tok->type != CLOSED_PAR)
    {
        printf("Missing parentheses\n");
        free(f);
        return NULL;
    }

    // Function body
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_BRACE)
    {
        printf("Missing bracket\n");
        free(f);
        return NULL;
    }

    // Parse statement
    statement *s= parseStatement();
    // If the statement couldn't be parsed return NULL
    if (s == NULL)
    {
        free(f);
        return NULL;
    }
    f->statement = s;
    
    tok = nextToken();
    if (tok == NULL || tok->type != CLOSED_BRACE)
    {
        printf("Missing bracket\n");
        free(f);
        return NULL;
    }
    return f;
}

// Returns null if something went wrong
statement *parseStatement()
{
    statement *s = malloc(sizeof(statement));
    if (s == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Check for return statement
    token *tok = nextToken();
    if (tok == NULL || tok->type != RETURN)
    {
        printf("Missing return statement\n");
        free(s);
        return NULL;
    }

    // Get expression
    // Start with the lowest precedence level
    expression *e = parseExpression(0);
    if (e == NULL)
    {
        free(s);
        return NULL;
    }
    s->exp = e;

    //Check for a semicolon
    tok = nextToken();
    if (tok == NULL || tok->type != SEMICOLON)
    {
        printf("Missing Semicolon\n");
        // Free the statement and any nested unOps
        freeStatement(s);
        return NULL;    
    }
    return s;
}

// bp = binding power
expression *parseExpression(int bp)
{
    //create new expression
    // nud
    expression *e = parseInitial();
    if (e == NULL)
    {
        printf("Error parsing initial exp\n");
        return NULL;
    }

    token *next = peekToken();
    if (next == NULL)
    {
        return e;
    }

    token *current;

    // Keep trying to create a new expression with e as the leftExp as long as the operator precedence is greater
    while (getOperatorPrecedence(next) > bp)
    {
        current = nextToken();
        e = parseLeftDenotation(e, current);
        if (e == NULL)
        {
            printf("Couldn't parse led\n");
            return NULL;
        }
        next = peekToken();
        // If there is no next token
        if (next == NULL)
        {
            return e;
        }
    }
    return e;
}

// operatorToken should be a binaryOp token
expression *parseLeftDenotation(expression *left, token *operatorToken)
{
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    e->expL = left;
    e->operator = operatorToken->lexeme[0];
    e->type = BINARY_OP;
    
    // Parse the right side expression with supplying the operatorPrecedence
    int precedence = getOperatorPrecedence(operatorToken);
    e->expR = parseExpression(precedence);
    if (e->expR == NULL)
    {
        printf("Error parsing expression\n");
        // The left part of the tree is already built fully so every element of it needs to be freed
        freeExpression(e->expL);
        free(e);
        return NULL;
    }

    return e;
}

expression *parseInitial()
{
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    token *tok = nextToken();
    if (tok == NULL)
    {
        printf("Missing expression\n");
        free(e);
        return NULL;
    }
    else if (tok->type == INT) // Const INT
    {
        e->type = INTEGER;
        e->value = tok->value;
        // Default values for the rest of the fields
        e->expL = NULL;
        e->expR = NULL;
        e->operator = '\0';
        e->unOp = NULL;
    }
    else if (tok->type == MINUS || tok->type == BITWISE_COMP || tok->type == LOGICAL_NEG) //UnaryOp
    {
        e->type = UNARY_OP;
        int value = -1;
        e->expL = NULL;
        e->expR = NULL;
        e->operator = '\0';
        // Parse for the unarOp
        e->unOp = parseUnaryOp(tok);
        if (e->unOp == NULL)
        {
            printf("Error parsing unaryOp\n");
            return NULL;
        }
    }
    else 
    {
        free(e);
        printf("Missing expression\n");
        return NULL; 
    }

    return e;
}

unaryOp *parseUnaryOp(token *tok)
{
    unaryOp *u = malloc(sizeof(unaryOp));
    if (u == NULL)
    {
        free(u);
        printf("Memory allocation failed\n");
        return NULL;
    }
    // Set the type to the first character of the lexeme
    // First character should be the operation while the 2nd character is \0
    u->operator = tok->lexeme[0];

    // Recursively parse to check for another unaryOp or const expression
    // Recursion should end when a const expression is met or all the tokens are consumed
    u->innerExp = parseInitial();
    if (u->innerExp == NULL)
    {
        printf("Error parsing unaryOp\n");
        free(u);
        return NULL;
    }

    return u;
}

int getOperatorPrecedence(token *tok)
{
    switch (tok->type)
    {
        case ADD:
            return 1;
        case MINUS:
            return 1;
        case MULTIPLY:
            return 2;
        case DIVIDE:
            return 2;
    }
    return -1;
}

void freeProgram(program *p)
{
    freeFunc(p->func);
    free(p);
}

void freeFunc(function *f)
{
    // The id points to an array of chars also pointed to by the node list
    // Free this chunk of memory is given to the freeTokens() function
    //free(f->id);
    freeStatement(f->statement);
    free(f);
}

void freeStatement(statement *s)
{
    freeExpression(s->exp);
    free(s);
}

void freeExpression(expression *e)
{
    if (e->type == BINARY_OP)
    {
        freeExpression(e->expL);
        freeExpression(e->expR);
    }
    else if (e->type == UNARY_OP)
    {
        freeUnaryOp(e->unOp);
    }
    free(e);
}

void freeUnaryOp(unaryOp *u)
{
    freeExpression(u->innerExp);
    free(u);
}