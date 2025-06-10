#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "parser.h"
#include "tokens.h"
#include "keywords.h"
#include "variables.h"



// map of variables 
// Memory is freed when expressions are free'd

program *parseProgram()
{
    program *p = malloc(sizeof(program));
    if (p == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    function *f = parseFunc();
    if (f == NULL)
    {
        free(p);
        return NULL;
    }
    p->func = f;

    return p;
}

// Returns NULL if something went wrong
function *parseFunc()
{
    function *f = malloc(sizeof(function));
    if (f == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    token *tok = nextToken();
    // For now functions can only be of type INT
    if (tok == NULL || tok->type != KEYWORD_INT)
    {
        printf("Missing function return type\n");
        free(f);
        return NULL;
    }
    
    // Look for and set function ID
    tok = nextToken();
    if (tok == NULL || tok->type != IDENTIFIER)
    {
        printf("Missing function name\n");
        free(f);
        return NULL;
    }
    // Point the pointer to the same memory as pointed to by the token's lexeme property
    // This memory is later freed by freeTokens() not freeFunc()
    f->id = tok->lexeme;

    // Parentheses
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_PAR)
    {
        printf("Missing parentheses\n");
        free(f);
        return NULL;

    }
    tok = nextToken();
    if (tok == NULL || tok->type != CLOSED_PAR)
    {
        printf("Missing parentheses\n");
        free(f);
        return NULL;
    }

    // Function body
    tok = nextToken();
    if (tok == NULL || tok->type != OPEN_BRACE)
    {
        printf("Missing bracket\n");
        free(f);
        return NULL;
    }
    
    // Starting number of statements is 100
    int maxNumStatements = 100;
    statement **statements = malloc(sizeof(statement)*maxNumStatements);
    if (statements == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Index for the next statement
    int snum = 0;
    // Parse statements until there are none left
    token *next = peekToken();
    if (next == NULL)
    {
        free(f);
        return NULL; 
    }

    while (next->type != CLOSED_BRACE)
    {
        statements[snum] = parseStatement();
        // If the statement couldn't be parsed return NULL
        if (statements == NULL)
        {
            // Free the remaining statements in the function
            f->numStatements = snum; // Used to traverse through the number of statements and free them
            f->statements = statements;
            freeFunc(f);
            printf("Failure to parse statement %d\n", snum);
            return NULL;
        }

        // Move to the next index for the next statement in the list
        snum++;

        // If the next statement would be out of range expand the buffer
        if (snum > maxNumStatements)
        {
            maxNumStatements+=100;
            statements = realloc(f->statements, sizeof(statement)*maxNumStatements);
            if (statements == NULL)
            {
                printf("Memory allocation failed\n");
                f->numStatements = snum; // Used to traverse through the number of statements and free them
                f->statements = statements;
                freeFunc(f);
                return NULL;
            }

        }
        next = peekToken();
        if (next == NULL)
        {
            f->numStatements = snum; // Used to traverse through the number of statements and free them
            f->statements = statements;
            freeFunc(f);
            printf("Failure to parse statement %d\n", snum);
            return NULL;
        }
    }

    // Finish supplying information for the function struct
    f->numStatements = snum;
    f->statements = statements;

    // Ensure that the last statement is a return statement
    if (f->statements[f->numStatements-1]->type != RETURN )
    {
        freeFunc(f);
        printf("Missing return statement\n");
        return NULL;
    }
    
    // Consume last CLOSED_BRACE token
    tok = nextToken();
    if (tok == NULL || tok->type != CLOSED_BRACE)
    {
        printf("Missing bracket\n");
        // Free the remaining statements in the function
        freeFunc(f);
        return NULL;
    }
    return f;
}

// Returns null if something went wrong
statement *parseStatement()
{
    statement *s = malloc(sizeof(statement));
    if (s == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Get next token
    token *tok = nextToken();
    if (tok == NULL)
    {
        printf("Missing return statement\n");
        free(s);
        return NULL;
    }
    expression *e;

    switch (tok->type) {
        // Integer definition
        case KEYWORD_INT:            
            s->type = ASSIGN;

            // Check for variable name
            tok = nextToken();
            if (tok == NULL)
            {
                printf("Missing variable name\n");
                free(s);
                return NULL;
            }
            if (tok->type != IDENTIFIER)
            {
                printf("Missing variable name\n");
                free(s);
                return NULL;
            }

            // Create the variable
            variable *v = malloc(sizeof(variable));
            if (v == NULL)
            {
                printf("Error allocating memory\n");
                free(s);
                return NULL;
            }

            // Set the variable name
            v->name = tok->lexeme;

            // Generate 
            v->id = generateVarId();
            
            // Set variable
            s->var = v;

            // Add variable to the map
            // If it fails then return
            if (addVariable(v) == EXIT_FAILURE)
            {
                free(v);
                free(s);
                return NULL;
            }
            

            // Check for equals sign
            tok = nextToken();
            if (tok == NULL)
            {
                printf("Missing assignment operator\n");
                free(s);
                free(v);
                return NULL;
            }
            if (tok->type != ASSIGN)
            {
                printf("Missing assignment operator\n");
                free(s);
                free(v);
                return NULL;
            }

            // Get the expression after the 
            e = parseExpression(0);
            if (e == NULL)
            {
                printf("Missing expression after assigment\n");
                // Free created structs
                free(s);
                free(v);
                return NULL;
            }
            s->exp = e;
            break;

        // Return statement
        case RETURN:
            s->type = RETURN;
            // Get expression
            // Start with the lowest precedence level
            e = parseExpression(0);
            if (e == NULL)
            {
                free(s);
                return NULL;
            }
            s->exp = e;
            s->var= NULL;
            break;
        
        // Normal expression
        default:
            s->type = EXP;
            s->var = NULL;
            // Get expression
            // Start with the lowest precedence level
            e = parseExpression(0);
            if (e == NULL)
            {
                free(s);
                return NULL;
            }
            s->exp = e;
            break;
    }
    
    

    //Check for a semicolon
    tok = nextToken();
    if (tok == NULL || tok->type != SEMICOLON)
    {
        printf("Missing Semicolon\n");
        // Free the statement and any nested unOps
        freeStatement(s);
        return NULL;    
    }
    return s;
}

// bp = binding power
expression *parseExpression(int bp)
{
    //create new expression
    // nud
    expression *e = parseInitial();
    if (e == NULL)
    {
        printf("Error parsing initial exp\n");
        return NULL;
    }

    token *next = peekToken();
    if (next == NULL)
    {
        return e;
    }

    token *current;

    // Check for semicolon
    if (next->type == SEMICOLON)
    {
        return e;
    }

    
    // Keep trying to create a new expression with e as the leftExp as long as the operator precedence is greater
    while (getOperatorPrecedence(next) > bp)
    {
        current = nextToken();
        // Left side of expression takes lower priority than the rightside
        e->binOp = parseBinaryOp(e, current); // Supply the current expression as the left part 
        e->type = BINARY_OP; // This changes it the first time then repeatedly changes it to binary_op redundantly after
        if (e->binOp == NULL) // Ensure that there wasn't an error parsing the binaryOp
        {
            printf("Couldn't parse led\n");
            // Free the expression itself
            free(e);
            return NULL;
        }
        next = peekToken();
        // If there is no next token
        if (next == NULL)
        {
            return e;
        }
    }
    return e;
}


// operatorToken should be a binaryOp token
binaryOp *parseBinaryOp(expression *left, token *operatorToken)
{
    binaryOp *b = malloc(sizeof(binaryOp));
    if (b == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Copy the left struct so that the original expression isn't affected
    expression *e = malloc(sizeof(expression));
    if (e == NULL)
    {
        printf("Memory allocation failed\n");
        free(b);
        return NULL;
    }
    e->value = left->value;
    e->type = left->type;
    e->binOp = left->binOp;
    e->unOp = left->unOp;

    // Set the left expression and the operator type
    b->expL = e;
    b->operator = operatorToken->type;
    
    // Parse the right side binaryOp with supplying the operatorPrecedence
    int precedence = getOperatorPrecedence(operatorToken);
    b->expR = parseExpression(precedence);
    if (b->expR == NULL)
    {
        printf("Error parsing binaryOp\n");
        // The left part of the tree is already built fully so every element of it needs to be freed
        freeExpression(b->expL);
        free(b);
        return NULL;
    }

    return b;
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
        e->type = INT;
        e->value = tok->value;
        // Default values for the rest of the fields
        e->binOp = NULL;
        e->unOp = NULL;
        e->var = NULL;
    }
    else if (tok->type == MINUS || tok->type == BITWISE_COMP || tok->type == LOGICAL_NEG) //UnaryOp
    {
        e->type = UNARY_OP;
        int value = -1;
        e->binOp = NULL;
        e->var = NULL;
        // Parse for the unarOp
        e->unOp = parseUnaryOp(tok);
        if (e->unOp == NULL)
        {
            printf("Error parsing unaryOp\n");
            return NULL;
        }
    }
    else if (tok->type == OPEN_PAR) // Parenthesized expression
    {
        // Free the expression already created
        free(e);
        // The new expression will be set to the expression created by parseExpression
        e = parseExpression(0); // Parse a new expression resetting the binding power to 0
        // Check for the matching closed paren
        tok = nextToken();
        if (tok == NULL || tok->type != CLOSED_PAR)
        {
            printf("Missing matching parentheses\n");
            freeExpression(e);
            return NULL;
        }
    }
    else if (tok->type == IDENTIFIER) {
        e->type = VARIABLE;
        e->binOp = NULL;
        e->value = -1;
        e->unOp = NULL;
        // Attempt to get the variable from the var_Map
        // It should already be defined if it is being referenced in an expression
        e->var = getVariable(tok->lexeme);
        if (e->var == NULL)
        {
            free(e);
            printf("Reference to uninstiantiated variable: %s\n", tok->lexeme);
            return NULL;
        }
    }
    else 
    {
        free(e);
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

int getOperatorPrecedence(token *tok)
{
    switch (tok->type)
    {
        case MULTIPLY:
            return 6;
        case DIVIDE:
            return 6;
        case ADD:
            return 5;
        case MINUS:
            return 5;
        case GREATER:
            return 4;
        case GREATER_EQUAL:
            return 4;
        case LESS:
            return 4;
        case LESS_EQUAL:
            return 4;
        case EQUALS:
            return 3;
        case NOT_EQUAL:
            return 3;
        case AND:
            return 2;
        case OR:
            return 1;
    }
    return -1;
}

void freeProgram(program *p)
{
    freeFunc(p->func);
    free(p);
}

// f->numStatements MUST BE DEFINED FOR THIS TO WORK
void freeFunc(function *f)
{
    // The id points to an array of chars also pointed to by the node list
    // Free this chunk of memory is given to the freeTokens() function
    //free(f->id);
    for (int i = 0; i < f->numStatements; i++)
    {
        // Make sure that the statement is only freed if it exists
        // Protects agaisnt invalid reads when the statement fails to parse and the structs need to be cleaned up
        if (f->statements[i] != NULL)
        {
            freeStatement(f->statements[i]);
        }
    }
    free(f->statements);
    free(f);
}

void freeStatement(statement *s)
{
    freeExpression(s->exp);
    free(s->var);
    free(s);
}

void freeExpression(expression *e)
{
    if (e->type == BINARY_OP)
    {
        freeBinaryOp(e->binOp);
    }
    else if (e->type == UNARY_OP)
    {
        freeUnaryOp(e->unOp);
    }
    free(e);
}

void freeBinaryOp(binaryOp *b)
{
    freeExpression(b->expL);
    freeExpression(b->expR);
    free(b);
}

void freeUnaryOp(unaryOp *u)
{
    freeExpression(u->innerExp);
    free(u);
}