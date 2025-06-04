#include <stdio.h>
#include "types.h"
#include "codegen.h"
#include "keywords.h"

// For the label generator to generate new labels
int counter = 0;

// Write a function in assembly
void writeFunction(FILE *outFile, function *f)
{
    char *funcName = f->id;
    fprintf(outFile, ".text\n.globl %s\n\n%s:\n", funcName, funcName);

    // Write each sequence one by one
    for (int i = 0; i < f->numStatements; i++)
    {
        writeStatement(outFile, f->statements[i]);
    }
    fprintf(outFile, "\tret"); // Functions require return statements at the end

}

// Write a basic return statement
void writeStatement(FILE *outFile, statement *s)
{
    writeExpression(outFile, s->exp);
}

// Write an integer or unaryOp expression
void writeExpression(FILE *outFile, expression *e)
{
    if (e->type == INT)
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
    
    if (b->operator == OR || b->operator == AND)
    {
        // Create clause2 and end labels
        int clause2 = getLabel();
        int end = getLabel();

        switch (b->operator)
        {
            case OR:
                // See if the 1st expression was false
                fprintf(outFile, "\tcmp $0, %%rax\n");

                // Number label for the 2nd clause of the OR statement (2nd expression)
                // Jump to right expression if the left is false
                fprintf(outFile, "\tje _%d\n", clause2);

                // If right exp is true then set rax to 1 and jump to end (short-circuit)
                fprintf(outFile, "\tmov $1, %%rax\n");
                // Number label for the end of the statement
                fprintf(outFile, "\tjmp _%d\n", end);

                // Write clause2 (evaulate the 2nd expression)
                fprintf(outFile, "_%d:\n", clause2);
                writeExpression(outFile, b->expR);

                // See if the right expression was false
                fprintf(outFile, "\tcmp $0, %%rax\n");
                fprintf(outFile, "\tmov $0, %%rax\n");

                // Set the al to if the right exp true or false
                fprintf(outFile, "\tsetne %%al\n");

                // Label next expression
                fprintf(outFile, "_%d:\n", end);
                break;
            case AND:
                // See if the 1st expression was false
                fprintf(outFile, "\tcmp $0, %%rax\n");

                // Jump if the first statement was true (check 2nd expression)
                fprintf(outFile, "\tjne _%d\n", clause2);
                // Number label for the end of the statement
                // Jump to next statement if the first statement was false (short-circuit)
                fprintf(outFile, "\tjmp _%d\n", end);

                // Write clause2 (evaulate the 2nd expression)
                fprintf(outFile, "_%d:\n", clause2);
                writeExpression(outFile, b->expR);

                // See if the right expression was false
                fprintf(outFile, "\tcmp $0, %%rax\n");
                fprintf(outFile, "\tmov $0, %%rax\n");

                // Set the al to if the right exp was true or false
                fprintf(outFile, "\tsetne %%al\n");

                // Label next expression
                fprintf(outFile, "_%d:\n", end);
                break;
        }
    }
    // Not AND or OR statement
    else {
        // Solve rightside and store value into rcx
        // Push the value generated in rax onto the stack
        fprintf(outFile, "\tpush %%rax\n");

        // Write the right side
        writeExpression(outFile, b->expR);

        // Pop value off the left side and store into rcx
        fprintf(outFile, "\tpop %%rcx\n");
        // Do the operator dependent operations
        switch(b->operator)
        {
            case ADD:
                fprintf(outFile, "\tadd %%rcx, %%rax\n");
                break;
            case MINUS:
                // Subtract rax from rcx (leftExp - rightExp)
                // Store it into rcx
                fprintf(outFile, "\tsub %%rax, %%rcx\n");
                // Move the result in rcx back into rax
                fprintf(outFile, "\tmov %%rcx, %%rax\n");
                break;
            case MULTIPLY:
                // The values in RCX and RAX don't have because order doesn't matter in multiplication
                // Extend the value in rax to be in RDX:RAX
                fprintf(outFile, "\tcqo\n");
                fprintf(outFile, "\timul %%rcx\n"); // Signed multiplication
                break;
            case DIVIDE:
                // Swap the values in RAX and RCX so that the left value is in RAX and the right value is in RCX
                fprintf(outFile, "\txchg %%rax, %%rcx\n");
                // Extend the number in rax by doubling its size so it takes up RDX:RAX
                fprintf(outFile, "\tcqo\n");
                fprintf(outFile, "\tidiv %%rcx\n"); // Signed division
                break;
            case EQUALS:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsete %%al\n");
                break; 

            case NOT_EQUAL:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsetne %%al\n");
                break;

            case GREATER:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsetg %%al\n");
                break;

            case GREATER_EQUAL:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsetge %%al\n");
                break;

            case LESS:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsetl %%al\n");
                break;

            case LESS_EQUAL:
                // Compare the two values of the expressions stored in rax and rcx (rcx-rax)
                fprintf(outFile, "\tcmp %%rax, %%rcx\n");
                // Zero out rax so the result can be set to the lower 8 bits
                fprintf(outFile, "\tmov $0, %%rax\n");
                // Set the result of the comparison to the lower 8 bits of rax
                fprintf(outFile, "\tsetle %%al\n");
                break;
        }
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

// Generates labels for jump points in the assembly
int getLabel()
{
    counter++;
    return counter;
}
