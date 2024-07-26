#ifndef PARSER_H
#define PARSER_H

#include "types.h"

program *parseProgram();
function *parseFunc();
statement *parseStatement();
expression *parseInitial();
expression *parseBinaryOp(expression *left, token *operatorToken);
expression *parseExpression(int bp);
unaryOp *parseUnaryOp(token *tok);
int getOperatorPrecedence(token *tok);
void freeProgram(program *p);
void freeFunc(function *f);
void freeStatement(statement *s);
void freeExpression(expression *e);
void freeUnaryOp(unaryOp *u);



#endif