#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>

#include "types.h"



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
void addSingleCharacterToken(token *tokens, int index, char *lexeme)
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
    }
    tokens[index].lexeme = malloc(sizeof(char) * (2));
    if (tokens[index].lexeme == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
    }
    strncpy(tokens[index].lexeme, lexeme, 2);
    tokens[index].value = -1;
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
    token *tokens = malloc(sizeof(token) * tokenList->maxSize);
    

    regex_t singleCharToken;
    // Make sure the minus sign is at the end because it has special effects when surrounded by characters
    if (regcomp(&singleCharToken, "[{}();~!+/-]", 0) != 0)
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
            c = fgetc(file);
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
            c = fgetc(file);
        }
        else // Single characters (punctuation and unitary operators)
        {
            buffer[0] = c;
            buffer[1] = '\0';
            // Add token if it matches punctuation
            if (regexec(&singleCharToken, buffer, 0, NULL, 0) == 0)
            {
                addSingleCharacterToken(tokens, index, buffer);
                index++;
            }

            // Get the next character to continue the loop
            c = fgetc(file);
        }
        // printf("%s", buffer); Check to see if it is reading through each character of the file

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
    regfree(&singleCharToken);
    free(buffer);

    // Set the rest of the tokenList attributes
    tokenList->list = tokens;
    tokenList->numTokens = index;
    tokenList->nextToken = -1;

    // Return the number of 
    return tokenList;
}