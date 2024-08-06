#include "types.h"
#include "pprint.h"
#include <stdio.h>

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
    printf(" %c ", b->operator);
    printExpression(b->expR);
    printf(")");
}

void printExpression(expression *e)
{
    if (e->type == BINARY_OP)
    {
        printBinaryOp(e->binOp);
       
    }
    else if (e->type == UNARY_OP)
    {
        printUnaryOp(e->unOp);
    }
    else if (e->type == INTEGER)
    {
        printf("%d", e->value);
    }
}
// All statements are return statements for now
void printStatement(statement *s)
{
    printf("\tRETURN ");
    printExpression(s->exp);
    printf("\n");
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