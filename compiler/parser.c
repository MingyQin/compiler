#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "parser.h"
#include "tokens.h"


program *parseProgram()
{
    program *p = malloc(sizeof(program));
    p->func = parseFunc();
    return p;
}

function *parseFunc()
{
    function *f = malloc(sizeof(function));
    token *tok = nextToken();
    // For now functions can only be of type INT
    if (tok == NULL || tok->type != KEYWORD_INT)
    {
        printf("Missing function return type\n");
        exit(1);
    }

    // Look for and set function ID
    tok = nextToken();
    if (tok == NULL || tok->type != IDENTIFIER)
    {
        printf("Missing function name\n");
        exit(1);
    }
    // Point the pointer to the same memory as pointed to by the token's lexeme property
    // This memory is later freed by freeTokens() not freeFunc()
    f->id = tok->lexeme;

    // Parentheses
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_PAR)
    {
        printf("Missing parentheses\n");
        exit(1);
    }
    tok = nextToken();
    if (tok == NULL || tok->type != CLOSED_PAR)
    {
        printf("Missing parentheses\n");
        exit(1);
    }

    // Function body
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_BRACE)
    {
        printf("Missing bracket\n");
        exit(1);
    }

    // Parse statement
    f->statement = parseStatement();
    tok = nextToken();


    if (tok == NULL || tok->type != CLOSED_BRACE)
    {
        printf("Missing bracket\n");
        exit(1);
    }
    return f;
}

statement *parseStatement()
{
    statement *s = malloc(sizeof(statement));

    // Check for return statement
    token *tok = nextToken();
    if (tok == NULL || tok->type != RETURN)
    {
        printf("Missing return statement\n");
        exit(1);
    }

    // Get expression
    s->exp = parseExpression();

    //Check for a semicolon
    tok = nextToken();
    if (tok == NULL || tok->type != SEMICOLON)
    {
        printf("Missing Semicolon\n");
        exit(1);
    }
    return s;
}

expression *parseExpression()
{
    expression *e = malloc(sizeof(expression));

    // Check for int value
    token *tok = nextToken();
    if (tok == NULL || tok->type != INT)
    {
        printf("Missing expression\n");
        exit(1);
    }
    e->value = tok->value;
    return e;
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
    free(e);
}