#ifndef PARSER_H
#define PARSER_H

#include "types.h"

program *parseProgram();
function *parseFunc();
statement *parseStatement();
expression *parseExpression();
void freeProgram(program *p);
void freeFunc(function *f);
void freeStatement(statement *s);
void freeExpression(expression *e);

#endif