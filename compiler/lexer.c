#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>

#include "types.h"
#include "lexer.h"



// Returns the length of the word
// Consumes characters including the first character not alphbetical
int scanWord(char first, char *buffer, FILE *file)
{
    int maxBufferSize = 100;
    int i = 0;
    char c = first;
    do 
    {
        buffer[i] = c;
        i++;
        // If the word length exceeds the buffer size
        if (i >= maxBufferSize)
        {
            maxBufferSize += 100;
            buffer = (char *) realloc(buffer, maxBufferSize+100);
            if (!buffer)
            {
                puts("Memory reallocation failed");
                exit(1);
            }
        }
        c = fgetc(file);
    } while((isalpha(c) || isalnum(c)) && c != EOF);
    buffer[i] = '\0';

    return i+1;
}

int scanNum(char first, char *buffer, FILE *file)
{
    int maxBufferSize = 100;
    int i = 0;
    char c = first;
    do 
    {
        buffer[i] = c;
        i++;
        // If the word length exceeds the buffer size
        if (i >= maxBufferSize)
        {
            maxBufferSize += 100;
            buffer = (char *) realloc(buffer, maxBufferSize+100);
            if (!buffer)
            {
                puts("Memory reallocation failed");
                exit(1);
            }
        }
        c = fgetc(file);
    } while(isdigit(c) && c != EOF);
    buffer[i] = '\0';

    return i+1;
}

void addLiteralToken(token *tokens, int index, char *lexeme, int lexemeLen)
{
    // Assign the token type
    if (strcmp(lexeme, "return") == 0)
    {
        tokens[index].type = RETURN;
    }
    else if (strcmp(lexeme, "int") == 0)
    {
        tokens[index].type = KEYWORD_INT;
    }
    else
    {
        tokens[index].type = IDENTIFIER;
    }

    // Assign the lexeme value
    tokens[index].lexeme = malloc(sizeof(char) * (lexemeLen));
    if (tokens[index].lexeme == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
    }
    // Copy the lexeme string into token struct
    strncpy(tokens[index].lexeme, lexeme, lexemeLen);

    //Asasign the value
    tokens[index].value = -1;
}

void addIntegerToken(token *tokens, int index, char *lexeme, int lexemeLen)
{
    // Assign the token type
    tokens[index].type = INT;

    // Assign the lexeme value
    tokens[index].lexeme = malloc(sizeof(char) * (lexemeLen));
    if (tokens[index].lexeme == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
    }
    strncpy(tokens[index].lexeme, lexeme, lexemeLen);

    //Asasign the value
    tokens[index].value = atoi(lexeme);
}

// WARNING !!!!
// WORKS ONLY IF THE LEXEME IS ONE CHARACTER AND THEN A NULL TERMINATOR
// ONLY FOR LEXEME WITH LENGTH OF 2
// Returns 1 if token wasn't added
// Returns 0 if token was added
int addSingleCharacterToken(token *tokens, int index, char *lexeme)
{
    switch (lexeme[0])
    {
        case '{':
            tokens[index].type = OPEN_BRACE;
            break;
        case '}':
            tokens[index].type = CLOSED_BRACE;
            break;
        case '(':
            tokens[index].type = OPEN_PAR;
            break;
        case ')':
            tokens[index].type = CLOSED_PAR;
            break;
        case ';':
            tokens[index].type = SEMICOLON;
            break;
        case '-':
            tokens[index].type = MINUS;
            break;
        case '~':
            tokens[index].type = BITWISE_COMP;
            break;
        case '!':
            tokens[index].type = LOGICAL_NEG;
            break;
        case '+':
            tokens[index].type = ADD;
            break;
        case '*':
            tokens[index].type = MULTIPLY;
            break;
        case '/':
            tokens[index].type = DIVIDE;
            break;
        case '<':
            tokens[index].type = LESS;
            break;
        case '>':
            tokens[index].type = GREATER;
            break;
        default:
            return 1;
    }
    tokens[index].lexeme = malloc(sizeof(char) * (2));
    if (tokens[index].lexeme == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
    }
    strncpy(tokens[index].lexeme, lexeme, 2);
    tokens[index].value = -1;

    // Sucessfully added token
    return 0;
}

/*
// Returns 1 if token wasn't added
// Returns 0 if token was added
*/
int addDoubleCharacterToken(token *tokens, int index, char first, char next)
{
    switch (first)
    {
        case '=':
            if (first == next) tokens[index].type = EQUALS;
            else return 1;
        case '|':
            if (first == next) tokens[index].type = OR;
            else return 1;
            break;
        case '&':
            if (first == next) tokens[index].type = AND;
            else return 1;
            break;
        case '!':
            if (next == '=') tokens[index].type = NOT_EQUAL;
            else return 1;
            break;
        case '>':
            if (next == '=') tokens[index].type = GREATER_EQUAL;
            else return 1;
            break;
        case '<':
            if (next == '=') tokens[index].type = LESS_EQUAL;
            else return 1;
            break;
        default:
            // Return 1 if none of the first characters are correct
            return 1;
    }
    tokens[index].lexeme = malloc(sizeof(char) * 3);
    if (tokens[index].lexeme == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
    }
    // Set the lexeme
    tokens[index].lexeme[0] = first;
    tokens[index].lexeme[1] = next;
    tokens[index].lexeme[2] = '\0';
    tokens[index].value = -1;
    return 0;


}

// Returns the amount of tokens created
// The last index with a token in it is return-1
// Lexes through the file adding tokens to the list
token_list *lexFile(FILE *file)
{
    token_list *tokenList = malloc(sizeof(token_list));
    if (tokenList == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    tokenList->maxSize = 100;

    // Temporary pointer for creating the token list
    token *tokens = calloc(tokenList->maxSize, sizeof(token));
    

    regex_t doubleCharToken; // possible characters that could be part of a two char sequence
    if (regcomp(&doubleCharToken, "[<>=&|!]", 0) != 0)
    {
        puts("Regex compilation failed");
        return NULL;
    }
    


    int index = 0;
    // Set the string buffer size for each word
    int maxBufferSize = 100;
    char *buffer = malloc(sizeof(char) * maxBufferSize);
    if (buffer == NULL)
    {
        puts("Memory allocation failed");
        return NULL;
    }

    // Read initial character
    char c = fgetc(file);

    // Loop over file
    while (c !=EOF)
    {
        // For keywords
        if (isalpha(c))
        {
            int wordLength = scanWord(c, buffer, file);
            addLiteralToken(tokens, index, buffer, wordLength);

            index++;

            // Get next char
            // Set the cursor back one to first character that scanWord consumed but didn't add to the buffer
            // So that no characters are skipped
            fseek(file, -1, SEEK_CUR);
        }  
        else if (isdigit(c)) // Numbers are stored as strings for now
        {
            int numLength = scanNum(c, buffer, file);
            addIntegerToken(tokens, index, buffer, numLength);
            index++;

            // Get next char
            // Set the cursor back one to first character that scanWord consumed but didn't add to the buffer
            // So that no characters are skipped
            fseek(file, -1, SEEK_CUR);
        }
        else // Single characters (punctuation and unitary operators)
        {
            buffer[0] = c;
            buffer[1] = '\0';
            
            // Characters needed for adding a doubleCharacterToken
            char first = c;
            char next = peekChar(file);
            // Check for a potential double character
            if (addDoubleCharacterToken(tokens, index, first, next) == 0)
            {
                // Consume the character that was peeked at by skipping it
                fseek(file, 1, SEEK_CUR);
                // Increment th index if a token was added
                index++;

            }
            else if (addSingleCharacterToken(tokens, index, buffer) == 0) // Only try adding a singleCharactertoken if a doubleCharacter wasn't added
            {   
                // If the token was added then increment index
                index++; 
            }
        }
        // printf("%s", buffer); Check to see if it is reading through each character of the file
        
        // Read next char to continue loop
        c = fgetc(file);

        // Check if the next token will be out of range
        if (index >= tokenList->maxSize)
        {
            tokenList->maxSize += 100;
            tokens = realloc(tokens, sizeof(token) * tokenList->maxSize);
            if (tokens == NULL)
            {
                printf("Couldn't reallocate memory\n");
                return NULL;
            }
        }
    }

    
    // Free buffer and regex
    regfree(&doubleCharToken);
    free(buffer);

    // Set the rest of the tokenList attributes
    tokenList->list = tokens;
    tokenList->numTokens = index;
    tokenList->nextToken = -1;

    // Return the number of 
    return tokenList;
}

char peekChar(FILE *file)
{
    // get the next character
    char next = fgetc(file);

    // Return the character to the stream so it can be read again
    ungetc(next, file);

    return next;
}