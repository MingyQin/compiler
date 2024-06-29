#include <stdio.h>
#include "types.h"
#include "codegen.h"


// Write a function in assembly
void writeFunction(FILE *outFile, function *f)
{
    char *funcName = f->id;
    fprintf(outFile, ".text\n.globl %s\n\n%s:\n", funcName, funcName);
    writeStatement(outFile, f->statement);
}

// Write a basic return statement
void writeStatement(FILE *outFile, statement *s)
{
    writeExpression(outFile, s->exp);
    fprintf(outFile, "\tret");
}

// Write an integer expression
void writeExpression(FILE *outFile, expression *e)
{
    fprintf(outFile, "\tmovl $%d, %%eax\n", e->value);
}
