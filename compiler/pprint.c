#include "types.h"
#include "pprint.h"
#include <stdio.h>
#include "keywords.h"


void printUnaryOp(unaryOp *u)
{
    printf("(UNARY_OP<%c> ", u->operator);
    if (u->innerExp != NULL)
    {
        printExpression(u->innerExp);
    }
    printf(")");
}

void printBinaryOp(binaryOp *b)
{
    printf("(");
    printExpression(b->expL);
    printf(" %s ", getOperator(b->operator));
    printExpression(b->expR);
    printf(")");
}

void printExpression(expression *e)
{
    switch (e->type)
    {
        case BINARY_OP:
            printBinaryOp(e->binOp);
            break;
        case UNARY_OP:
            printUnaryOp(e->unOp);
            break;
        case INT:
            printf("%d", e->value);
            break;
        case VARIABLE:
            printf("%s", e->var->name);
            break;
    }
}
// All statements are return statements for now
void printStatement(statement *s)
{
    switch (s->type)
    {
        case RETURN:
            printf("\tRETURN ");
            printExpression(s->exp);
            printf("\n");
            break;
        case DECLARE:
            printf("\tINT ");
            printf("%s = ", s->var->name);
            printExpression(s->exp);
            printf(";\n");
            break;
        case ASSIGN:
            printf("%s = ", s->var->name);
            printExpression(s->exp);
            printf(";\n");
            break;
    }
    
}

void printFunction(function *f)
{
    if (f->id == NULL) 
    {
        printf("Missing function id\n");
    }
    else 
    {
        printf("FUNC %s:\n\tbody:\n", f->id);
        for (int i = 0; i < f->numStatements; i++)
        {
            printStatement(f->statements[i]);
        }
    }
}   

// Ensure that program *p != NULL before calling
void printProgram(program *p)
{
    if (p->func != NULL)
    {
        printFunction(p->func);
    }
}


// Converts a token_type to the character operator (can be up to a 2 character operator)
// Returns ? if the operator is unknown
char *getOperator(token_type operator)
{
    char *characters;
    switch (operator)
    {
        case ADD:
            characters = "+";
            break;
        case MINUS:
            characters = "+";
            break;
        case MULTIPLY:
            characters = "+";
            break;
        case DIVIDE:
            characters = "+";
            break;
        case AND:
            characters = "&&";
            break;
        case OR:
            characters = "||";
            break;
        case NOT_EQUAL:
            characters = "!=";
            break;
        case EQUALS:
            characters = "==";
            break;
        case LESS_EQUAL:
            characters = "<=";
            break;
        case GREATER_EQUAL:
            characters = ">=";
            break;
        case GREATER:
            characters = ">";
            break;
        case LESS:
            characters = "<";
            break;
        default:
            characters = "?";
            break;
    }
    return characters;
}