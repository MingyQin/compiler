#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "types.h"

void writeFunction(FILE* outFile, function *f);
void writeStatement(FILE *outFile, statement *s);
void writeExpression(FILE *outFile, expression *e);

#endif