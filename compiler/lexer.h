#ifndef LEXER_H
#define LEXER_H

#include "types.h"
#include <stdio.h>

void addLiteralToken(token *tokens, int index, char *lexeme, int lexemeLen);
void addIntegerToken(token *tokens, int index, char *lexeme, int lexemeLen);
int addSingleCharacterToken(token *tokens, int index, char *lexeme);
void addDoubleCharacterToken(token *tokens, int index, char first, char next);
char peekChar(FILE *file);
int scanWord(char first, char *buffer, FILE *file);
int scanNum(char first, char *buffer, FILE *file);
token_list *lexFile(FILE *file); 

#endif