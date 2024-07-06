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

// Write an integer or unaryOp expression
void writeExpression(FILE *outFile, expression *e)
{
    if (e->type == CONST)
    {
        // Move a 32-bit integer into the RAX register
        fprintf(outFile, "\tmovl $%d, %%eax\n", e->value);
    }
    else if (e->type == UNARY_OP)
    {
        writeUnaryOp(outFile, e->unOp);
    }
}

void writeUnaryOp(FILE *outFile, unaryOp *u)
{
    writeExpression(outFile, u->innerExp);
    switch (u->type)
    {
        case '-':
            fprintf(outFile, "\tneg %%rax\n");
            break;
        case '~':
            fprintf(outFile, "\tnot %%rax\n");
            break;
        case '!':
            // Compare the INT in EAX to 0
            fprintf(outFile, "\tcmp $0, %%rax\n");
            // Zero out all 64 bits of the register (32 bit operations extend to zero out the rest of the register)
            // Zeros the registers without affecting the ZF
            fprintf(outFile, "\tmov $0, %%eax\n");
            // Set the lowest 2 bits of RAX to the result of the comparison
            fprintf(outFile, "\tsete %%al\n");
            break;
    }
}

void writeProgram(FILE *outFile, program *p)
{
    writeFunction(outFile, p->func);
}