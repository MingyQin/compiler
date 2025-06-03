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
    if (e->type == INTEGER)
    {
        // Move a 32-bit integer into the RAX register
        fprintf(outFile, "\tmov $%d, %%eax\n", e->value);
    }
    else if (e->type == UNARY_OP)
    {
        writeUnaryOp(outFile, e->unOp);
    }
    else if (e->type == BINARY_OP)
    {
        writeBinaryOp(outFile, e->binOp);
    }
}

void writeBinaryOp(FILE *outFile, binaryOp *b)
{
    // Write left side
    writeExpression(outFile, b->expL);
    
    // Do the operator dependent operations
    switch(b->operator)
    {
        case ADD:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            fprintf(outFile, "\tadd %%rcx, %%rax\n");
            break;
        case MINUS:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Subtract rax from rcx (leftExp - rightExp)
            // Store it into rcx
            fprintf(outFile, "\tsub %%rax, %%rcx\n");
            // Move the result in rcx back into rax
            fprintf(outFile, "\tmov %%rcx, %%rax\n");
            break;
        case MULTIPLY:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // The values in RCX and RAX don't have because order doesn't matter in multiplication
            // Extend the value in rax to be in RDX:RAX
            fprintf(outFile, "\tcqo\n");
            fprintf(outFile, "\timul %%rcx\n"); // Signed multiplication
            break;
        case DIVIDE:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Swap the values in RAX and RCX so that the left value is in RAX and the right value is in RCX
            fprintf(outFile, "\txchg %%rax, %%rcx\n");
            // Extend the number in rax by doubling its size so it takes up RDX:RAX
            fprintf(outFile, "\tcqo\n");
            fprintf(outFile, "\tidiv %%rcx\n"); // Signed division
            break;
        case EQUALS:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsete %%al\n");
            break; 

        case NOT_EQUAL:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsetne %%al\n");
            break;

        case GREATER:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsetg %%al\n");
            break;

        case GREATER_EQUAL:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsetge %%al\n");
            break;

        case LESS:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsetl %%al\n");
            break;

        case LESS_EQUAL:
            // Push the value generated in rax onto the stack
            fprintf(outFile, "\tpush %%rax\n");

            // Write the right side
            writeExpression(outFile, b->expR);

            // Pop value off the left side and store into rcx
            fprintf(outFile, "\tpop %%rcx\n");

            // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
            fprintf(outFile, "\tcmp %%rax, %%rcx\n");
            // Zero out rax so the result can be set to the lower 8 bits
            fprintf(outFile, "\tmov $0, %%rax\n");
            // Set the result of the comparison to the lower 8 bits of rax
            fprintf(outFile, "\tsetle %%al\n");
            break;
        
        case OR:
            fprintf(outFile, "$0, %%rax");
            fprintf(outFile, "jmp _");
            fprintf(outFile, "$0, %%rax");
            break;
    }
}


void writeUnaryOp(FILE *outFile, unaryOp *u)
{
    writeExpression(outFile, u->innerExp);
    switch (u->operator)
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