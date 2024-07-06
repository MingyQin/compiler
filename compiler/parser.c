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
    expression *e = parseExpression();
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

expression *parseExpression()
{
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Check for int value or unary operator
    token *tok = nextToken();
    if (tok == NULL)
    {
        printf("Missing expression\n");
        free(e);
        return NULL;
    }
    else if (tok->type == INT) // Const INT
    {
        e->type = CONST;
        e->value = tok->value;
        e->unOp = NULL;
    }
    else if (tok->type == MINUS || tok->type == BITWISE_COMP || tok->type == LOGICAL_NEG) //UnaryOp
    {
        e->type = UNARY_OP;
        int value = -1;
        // Parse for the unarOp
        e->unOp = parseUnaryOp(tok);
        if (e->unOp == NULL)
        {
            printf("Error parsing unaryOp\n");
            free(e);
            return NULL;
        }
    }
    else 
    {
        printf("Missing expression\n");
        free(e);
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
    u->innerExp = parseExpression();
    if (u->innerExp == NULL)
    {
        printf("Error parsing unaryOp\n");
        free(u);
        return NULL;
    }

    return u;
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
    // If it is a UNARY_OP then free the unaryOp first
    if (e->type == UNARY_OP)
    {
        freeUnaryOp(e->unOp);
    }
    free(e);
}

void freeUnaryOp(unaryOp *unOp)
{
    // Free the inner expressions
    freeExpression(unOp->innerExp);
    free(unOp);
}