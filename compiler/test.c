#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "lexer.h"

void freeTokens(token *tokens, int n);
int lexFile(FILE *file, token *tokens);


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
    token *tokens = malloc(sizeof(token) * 100);
    if (tokens == NULL)
    {
        puts("Memory allocation failed");
        return 1;
    }

    int num_Tokens = lexFile(file, tokens);

    printf("\n");
    // Print tokens
    for (int i = 0; i <= num_Tokens; i++)
    {
        printf("[%d, \"%s\", %d], ", tokens[i].type, tokens[i].lexeme, tokens[i].value);
    }
    printf("\n");

    // Free stuff

    // Free the tokens
    freeTokens(tokens, num_Tokens);

    // Free memory used by the regex

    printf("\n");
    fclose(file);
    return 0;
}

// Free all tokens and the elements contained inside the struct
void freeTokens(token *tokens, int n)
{
    for (int i = 0; i <= n; i++)
    {
        free(tokens[i].lexeme);
    }

    free (tokens);
}


// Returns the amount of tokens created
int lexFile(FILE *file, token *tokens)
{
    regex_t reegex;
    if (regcomp(&reegex, "[{}();]", 0) != 0)
    {
        puts("Regex compilation failed");
    }

    int index = 0;
    // Set the string buffer size for each word
    int maxBufferSize = 100;
    char *buffer = malloc(sizeof(char) * maxBufferSize);
    if (buffer == NULL)
    {
        puts("Memory allocation failed");
        exit(1);
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
        else if (isalnum(c)) // Numbers are stored as strings for now
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
        else // Anything that isn't made up letters
        {
            buffer[0] = c;
            buffer[1] = '\0';
            // Add token if it matches the regex
            if (regexec(&reegex, buffer, 0, NULL, 0) == 0)
            {
                addPunctuationToken(tokens, index, buffer);
                index++;
            }

            // Get the next character to continue the loop
            c = fgetc(file);
        }
        printf("%s", buffer);
    }
    regfree(&reegex);
    free(buffer);
    return index;
}