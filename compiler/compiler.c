#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "lexer.h"


void freeTokens(token *tokens, int n);
token *nextToken();

program *parseProgram();
function *parseFunc();
statement *parseStatement();
expression *parseExpression();

void freeProgram(program *p);
void freeFunc(function *f);
void freeStatement(statement *s);
void freeExpression(expression *e);



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

   

    printf("\n");
    // Print tokens
    for (int i = 0; i < num_tokens; i++)
    {
        printf("[%d, \"%s\", %d], ", tokens[i].type, tokens[i].lexeme, tokens[i].value);
    }
    printf("\n");


    // Parse the tokens into an AST

    // Root node of the AST
    program *ast = parseProgram();


    // Free the tokens 
    freeTokens(tokens, num_tokens);

    // Free the AST
    freeProgram(ast);

    printf("\n");

    fclose(file);
    return 0;
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

program *parseProgram()
{
    program *p;
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
        printf("Missing expression");
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
    free(f->id);
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