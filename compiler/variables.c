#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// List of variables
static varMap var_Map;

// Variable for the next VarId 
// 1st variable has id of 1
// Also serves as the index of variable struct in the varMap
int nextVarId = -1;
// Also used by getIndex to loop through each variable

// Create var_Map
int initVariables()
{
    var_Map.maxSize = 100;
    var_Map.vars = malloc(sizeof(variable)*var_Map.maxSize);
    if (var_Map.vars == NULL)
    {
        printf("Error allocating memory for varMap\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void freeVariables()
{
    free(var_Map.vars);
}

// Get index of variable using the variable identifier
variable* getVariable(char *id)
{
    for (int i = 0; i <= nextVarId; i++)
    {
        if (strcmp(var_Map.vars[i]->name, id) == 0)
        {
            return var_Map.vars[i];
        }
    }
    return NULL;
}

int addVariable(variable *v)
{
    // Resize the var_Map if it is over the max size
    if (v->id > var_Map.maxSize)
    {
        var_Map.maxSize += 100;
        var_Map.vars = realloc(var_Map.vars, sizeof(variable)*var_Map.maxSize);
    }
    if (var_Map.vars == NULL)
    {
        return EXIT_FAILURE;
    }

    var_Map.vars[v->id] = v;
    return EXIT_SUCCESS;
}

int generateVarId()
{
    nextVarId++;
    return nextVarId;
}