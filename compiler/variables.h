#ifndef VARIABLES_H
#define VARIABLES_H
#include "types.h"

int initVariables();
int addVariable(variable *v);
int generateVarId();
variable* getVariable(char *id);
void freeVariables();
variable* createVariable(char *name);

#endif