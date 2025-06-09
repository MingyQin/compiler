compiler: compiler.c lexer.c parser.c codegen.c pprint.c types.h
	gcc -g -o compiler compiler.c lexer.c parser.c codegen.c pprint.c variables.c

test: test.c lexer.c
	gcc -g -o test test.c lexer.c