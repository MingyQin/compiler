#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "lexer.h"
#include "parser.h"
#include "tokens.h"
#include "codegen.h"
#include "pprint.h"

// Used for getting the next token in the nextToken() function
static token_list *tokens;

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
    
    

    tokens = lexFile(file);
    if (tokens == NULL)
    {
        puts("Failed to create token list\n");
        return 1;
    }
   

    // Print tokens
    //printTokens();

    // Parse the tokens into an AST

    // Root node of the AST
    program *ast = parseProgram();
    if (ast == NULL)
    {
        printf("Couldn't create AST\n");
        freeTokens(tokens);
        fclose(file);
        return 1;
    }

    // Prettyprint the AST
    //printProgram(ast);

    // Code generation

    // Create a pointer to the filename (program.c --> program)
    // Copy the program name to a new string leaving out the last two characters
    char *outFileName = argv[1];

    // Replace the last two characters with ".s"
    outFileName[strlen(outFileName)-2] = '.';
    outFileName[strlen(outFileName)-1] = 's';

    printf("%s\n", outFileName);

    // Open new file
    FILE *outFile = fopen(outFileName, "w");
    if (outFile== NULL)
    {
        printf("Couldn't open outfile\n");
        return 1;
    }

    
    // Traverse the AST
    writeFunction(outFile, ast->func);

    // Ensure the file ends with a newline
    fprintf(outFile, "\n");

    // Free the tokens 
    freeTokens(tokens);
    //free(outFileName);

    // Free the AST
    freeProgram(ast);

    fclose(file);
    fclose(outFile);
    return 0;
}

// Free all tokens and the elements contained inside the struct
void freeTokens(token_list *tokens)
{
    for (int i = 0; i < tokens->numTokens; i++)
    {
        free(tokens->list[i].lexeme);
    }
    free(tokens->list);
    free (tokens);
}

// Returns a pointer to the next token from the token list
// Returns NULL if there is no next token
token *nextToken()
{
    tokens->nextToken++;
    if (tokens->nextToken >= tokens->numTokens)
    {
        return NULL;
    }
    return &tokens->list[tokens->nextToken];
}

// Returns a pointer to the next token from the list without incrementing nextToken
// Returns NULL if there is no next token
token *peekToken()
{
    if (tokens->nextToken+1 >= tokens->numTokens)
    {
        return NULL;
    }
    return &tokens->list[tokens->nextToken+1];
}

// Print the list of tokens
void printTokens()
{
    for (int i = 0; i < tokens->numTokens; i++)
    {
        printf("[%d, \"%s\", %d], ", tokens->list[i].type, tokens->list[i].lexeme, tokens->list[i].value);
    }
    printf("\n");
}