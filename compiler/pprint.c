#include "types.h"
#include "pprint.h"
#include <stdio.h>

void printUnaryOp(unaryOp *u)
{
    printf("UNARY_OP<%c> ", u->operator);
    if (u->innerExp != NULL)
    {
        printExpression(u->innerExp);
    }
}

void printExpression(expression *e)
{
    if (e->type == CONST)
    {
        printf("INT<%d>\n", e->value);
    }
    else if (e->type == UNARY_OP && e->unOp != NULL)
    {
        printUnaryOp(e->unOp);
    }
}

// All statements are return statements for now
void printStatement(statement *s)
{
    printf("\tRETURN ");
    printExpression(s->exp);
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
        if (f->statement != NULL)
        {
            printStatement(f->statement);
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