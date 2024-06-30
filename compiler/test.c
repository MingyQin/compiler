#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "types.h"


int main(int argc, char *argv[])
{
    // Print the change
    unaryOp *unOp = malloc(sizeof(unaryOp));
    unOp->innerExp = malloc(sizeof(expression));
    expression *exp = unOp->innerExp;
    exp->type = CONST;
    exp->value = 1;
    exp->unOp = NULL;

    printf("%d, %d\n", exp->type, exp->value);

    free(exp);
    free(unOp);

}