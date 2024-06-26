#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


typedef struct {
    int value;
} expression;

typedef struct {
    expression *exp;
} statement;

typedef struct {
    char *id;
    statement *statement;
} function;

typedef struct {
    function *func;
} program;

int main(int argc, char *argv[])
{
    program ast;
    // Create enough size for 2 functions
    ast.func = malloc(sizeof(function) *2);

    // Access the actual memory stored at the locations
    ast.func[0].id = "main";
    ast.func[1].id = "fail";

    // Make pointer f point to the location of the function stored at index 1 of the ast.func
    function *f = &ast.func[1];
    // Change the value of ast.func[1] to be main
    f->id = "main";

    // Print the change
    printf("%s\n", ast.func[1].id);

    free(ast.func);
}