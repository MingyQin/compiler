compiler: compiler.c lexer.c parser.c codegen.c pprint.c
	gcc -o compiler compiler.c lexer.c parser.c codegen.c pprint.c

test: test.c lexer.c
	gcc -o test test.c lexer.c