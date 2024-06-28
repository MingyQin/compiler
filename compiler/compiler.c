#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "lexer.h"
#include "parser.h"
#include "tokens.h"

void writeFunction(FILE* outFile, function *f);
void writeStatement(FILE *outFile, statement *s);
void writeExpression(FILE *outFile, expression *e);




// Used for getting the next token in the nextToken() function
static int token_index = -1;
static int num_tokens = 0;
static token *tokens;

int main(int argc, char *argv[])
{
    // Ensure there are enough arguments
    if (argc != 2)
    {
        puts("Not enough arguments supplied");
        puts("Usage: ./lexer filename");
        return 1;
    }

    // Open file conneciton
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("File \"%s\" not found\n", argv[1]);
        return 1;
    }
    

    // Create token list
    // MAX TOKENS IS 100 (Could cause errors later)
    tokens = malloc(sizeof(token) * 100);
    if (tokens == NULL)
    {
        puts("Memory allocation failed");
        return 1;
    }

    num_tokens = lexFile(file, tokens);

   

    //printf("\n");
    // Print tokens
    /*for (int i = 0; i < num_tokens; i++)
    {
        printf("[%d, \"%s\", %d], ", tokens[i].type, tokens[i].lexeme, tokens[i].value);
    }
    printf("\n");*/


    // Parse the tokens into an AST

    // Root node of the AST
    program *ast = parseProgram();
    if (ast == NULL)
    {
        printf("Couldn't create AST\n");
        freeTokens(tokens, num_tokens);
        fclose(file);
        return 1;
    }

    // Code generation

    // Open new file
    FILE *outFile = fopen("result.s", "w");
    
    // Traverse the AST
    writeFunction(outFile, ast->func);

    // Free the tokens 
    freeTokens(tokens, num_tokens);

    // Free the AST
    freeProgram(ast);

    fclose(file);
    fclose(outFile);
    return 0;
}

// Write a function in assembly
void writeFunction(FILE *outFile, function *f)
{
    char *funcName = f->id;
    fprintf(outFile, ".text\n.globl %s\n\t%s:\n", funcName, funcName);
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
    fprintf(outFile, "\tmovl $%d, %%eax", e->value);
}


// Free all tokens and the elements contained inside the struct
void freeTokens(token *tokens, int num_tokens)
{
    for (int i = 0; i < num_tokens; i++)
    {
        free(tokens[i].lexeme);
    }

    free (tokens);
}

// Returns a pointer to the next token from the token list
// Returns NULL if there is no next token
token *nextToken()
{
    token_index++;
    if (token_index > num_tokens)
    {
        return NULL;
    }
    return &tokens[token_index];
}