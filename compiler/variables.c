#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variables.h"

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
    for (int i = 0; i <= nextVarId; i++)
    {
        free(var_Map.vars[i]);
    }
    free(var_Map.vars);
}

// Get index of variable using the variable identifier
variable* getVariable(char *name)
{
    for (int i = 0; i <= nextVarId; i++)
    {
        if (strcmp(var_Map.vars[i]->name, name) == 0)
        {
            return var_Map.vars[i];
        }
    }
    return NULL;
}

// Create a variable from its identifier
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
        printf("Error reallocating memory\n");
        return EXIT_FAILURE;
    }

    var_Map.vars[v->id] = v;
    return EXIT_SUCCESS;
}

// Create variable from variable name
variable* createVariable(char *name)
{
    variable *v = malloc(sizeof(variable));
    if (v == NULL)
    {
        printf("Error allocating memory\n");
        return NULL;
    }

    // Check if variable exists already
    if (getVariable(name) != NULL)
    {
        printf("Variable %s is already declared\n", name);
        free(v);
        return NULL;
    }
    
    // Create variable information
    v->id = generateVarId();
    v->name = name;

    // Add variable to the map
    addVariable(v);
    return v;
}

int generateVarId()
{
    nextVarId++;
    return nextVarId;
}