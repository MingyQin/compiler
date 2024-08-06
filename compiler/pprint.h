#ifndef PPRINT_H
#define PPRINT_H
#include "types.h"

void printUnaryOp(unaryOp *u);
void printExpression(expression *e);
void printStatement(statement *s);
void printFunction(function *f);
void printProgram(program *p);
void printBinaryOp(binaryOp *b);

#endif
