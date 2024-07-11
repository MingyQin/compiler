#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "tokens.h"

// Experimental pratt parser
struct expression;


/*typedef struct {
    char operator;
    struct expression *innerExp;
} unaryOp;*/

typedef struct expression {
    enum {
        BINARY_OP, INTEGER, UNARY_OP
    } type;
    char operator;
    int value;
    unaryOp *unOp;
    struct expression *expL;
    struct expression *expR; 
} expression;

static token_list *tokens;

expression *parseInitial();
expression *parseLeftDenotation(expression *left, token *operatorToken);
expression *parseExpression(int bp);
void printTree(expression *e);
void freeTree(expression *e);
int getOperatorPrecedence(token *tok);
unaryOp *parseUnaryOp(token *tok);
void freeUnary(unaryOp *u);
void printUnaryOp(unaryOp *u);

int main(int argc, char *argv[])
{    
    printf("Hello\n");
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
    for (int i = 0; i < tokens->numTokens; i++)
    {
        printf("[%d, \"%s\", %d], ", tokens->list[i].type, tokens->list[i].lexeme, tokens->list[i].value);
    }
    printf("\n");

    // Create shitty AST
    // Start with the lowest precedence level
    expression *tree = parseExpression(0);

    printTree(tree);
    printf("\n");

    freeTree(tree);
    freeTokens(tokens);

    fclose(file);
}

// bp = binding power
expression *parseExpression(int bp)
{
    //create new expression
    // nud
    expression *e = parseInitial();

    token *next = peekToken();
    if (next == NULL)
    {
        return e;
    }

    token *current;

    while (getOperatorPrecedence(next) > bp)
    {
        current = nextToken();
        e = parseLeftDenotation(e, current);
        next = peekToken();
        if (next == NULL)
        {
            return e;
        }
    }
    return e;
}

// operatorToken should be a binaryOp token
expression *parseLeftDenotation(expression *left, token *operatorToken)
{
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    e->expL = left;
    e->operator = operatorToken->lexeme[0];
    e->type = BINARY_OP;
    
    // Parse the right side expression with supplying the operatorPrecedence
    int precedence = getOperatorPrecedence(operatorToken);

    e->expR = parseExpression(precedence);

    return e;
}

expression *parseInitial()
{
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    token *tok = nextToken();
    if (tok == NULL)
    {
        printf("Missing expression\n");
        free(e);
        return NULL;
    }
    else if (tok->type == INT) // Const INT
    {
        e->type = INTEGER;
        e->value = tok->value;
        // Default values for the rest of the fields
        e->expL = NULL;
        e->expR = NULL;
        e->operator = '\0';
        e->unOp = NULL;
    }
    else if (tok->type == MINUS || tok->type == BITWISE_COMP || tok->type == LOGICAL_NEG) //UnaryOp
    {
        e->type = UNARY_OP;
        int value = -1;
        e->expL = NULL;
        e->expR = NULL;
        e->operator = '\0';
        // Parse for the unarOp
        e->unOp = parseUnaryOp(tok);
        if (e->unOp == NULL)
        {
            printf("Error parsing unaryOp\n");
            return NULL;
        }
    }
    else 
    {
        printf("Missing expression\n");
        return NULL; 
    }

    return e;
}

unaryOp *parseUnaryOp(token *tok)
{
    unaryOp *u = malloc(sizeof(unaryOp));
    if (u == NULL)
    {
        free(u);
        printf("Memory allocation failed\n");
        return NULL;
    }
    // Set the type to the first character of the lexeme
    // First character should be the operation while the 2nd character is \0
    u->operator = tok->lexeme[0];

    // Recursively parse to check for another unaryOp or const expression
    // Recursion should end when a const expression is met or all the tokens are consumed
    u->innerExp = parseInitial();
    if (u->innerExp == NULL)
    {
        printf("Error parsing unaryOp\n");
        free(u);
        return NULL;
    }

    return u;
}


void printTree(expression *e)
{
    if (e->type == BINARY_OP)
    {
        printf("(");
        printTree(e->expL);
        printf(" %c ", e->operator);
        printTree(e->expR);
        printf(")");
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

void printUnaryOp(unaryOp *u)
{
    printf("(");
    printf("%c", u->operator);
    printTree(u->innerExp);
    printf(")");
}

// Free tree of expressions
void freeTree(expression *e)
{
    if (e->type == BINARY_OP)
    {
        freeTree(e->expL);
        freeTree(e->expR);
    }
    else if (e->type == UNARY_OP)
    {
        freeUnary(e->unOp);
    }
    free(e);
}

void freeUnary(unaryOp *u)
{
    freeTree(u->innerExp);
    free(u);
}


// Return -1 if it is not a binaryOp
// Otherwise return the precedence value
int getOperatorPrecedence(token *tok)
{
    switch (tok->type)
    {
        case ADD:
            return 1;
            break;
        case MINUS:
            return 1;
            break;
        case MULTIPLY:
            return 2;
            break;
        case DIVIDE:
            return 2;
            break;
    }
    return -1;
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

