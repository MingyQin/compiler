# Write a C Compiler!

**This repo is no longer active. The latest version of this test suite, which accompanies the [Writing a C Compiler book](https://nostarch.com/writing-c-compiler), is available [here](https://github.com/nlsandler/writing-a-c-compiler-tests/).**

This is a set of C test programs to help you write your own compiler. They were written to accompany [this tutorial](https://norasandler.com/2017/11/29/Write-a-Compiler.html).

## Usage

### test all
```
./test_compiler.sh /path/to/your/compiler
```

### test specific stages
To test stage 1 and stage 3,
```
./test_compiler.sh /path/to/your/compiler 1 3
```
To test from stage 1 to stage 6,
```
./test_compiler.sh /path/to/your/compiler `seq 1 6`
```

In order to use this script, your compiler needs to follow this spec:

1. It can be invoked from the command line, taking only a C source file as an argument, e.g.: `./YOUR_COMPILER /path/to/program.c`

2. When passed `program.c`, it generates executable `program` in the same directory.

3. It doesn’t generate assembly or an executable if parsing fails (this is what the test script checks for invalid test programs).

The script doesn’t check whether your compiler outputs sensible error messages, but you can use the invalid test programs to test that manually.

## Contribute

Additional test cases welcome! You can also file issues here, either about the test suite itself or about the content of the tutorial.

## Pratt Parsing (Precedence Climbing) Tutorial 
How to know which is grouped together?
    The most nested operation is done first. For example, in (a+(b-c)), b-c will be calculated together because they are grouped within the same expression. It doesn't matter if it is the leftExp or rightExp of the BinaryOp struct because, since they are in the same expression, they will always be calculated together before any other operation is carried out. 

Basic Parsing
    Parses with right associativity (groups expressions as far right as possible). a - b - c is parsed as a - (b - c). Why? Because the expression parser continuously calls parseBinaryOp() until it reaches the end of the list of tokens (when there are no more operator tokens). Each time a new operator is reached it will call parseBinaryOp again to redefine the last expression of the chain as the one involving the latest operator. The last time parseBinaryOp is called is the last binaryOperation of the sequence and starts a chain of returns that groups with rightwards associativity since each return call is just grouping a new right expression to the original last expression of the sequence. For ex: in a - b.... - x -z, the last binaryOp is (x-z) so it will be the final call of parseBinaryOp() and also the first return. Each subsequent return will just add on the previous left term to the binaryOp chain. Therefore, it will return (a - (b - ....... - (x - z))). 

Pratt Parsing
    Attempts to use to loops and recursion to parse with left and right associativity depending on the operator given.
    Basic Approach:
        1. Parse Initial expression 
        2. Check next operator
        3. Parse BinOp
            a. Initially parse the next integer after the operator
            b. While the next operator is higher precedence parse again (right associativity behavior)
                - The previously parsed integer becomes the leftSide and the right side is parsed through parseBinOp with the next operator precedence
                - Isolates the right-side into it's own expression -> by grouping them together it demonstrates higher precedence in order of operations
            c. If the next operator is lower precedence don't parse any further than just the rightExp integer and pop the expression off the stack (left associativity)
    - Everytime you call parseBinaryOp is follows the precedence of the operator associated with it. 
    - Call with precedence 0 initially - so that once everything returns you can keep parsing off the lowest precedence operators (+ and -)
    - Looping = left associtivity 
        - While loops 
        - Continuously adds onto the epxression on the rightside
        - Ex: 2+2+2 -> (2+2)+2
    - Recursion = right associativity 
        - Redefines and isolates the leftSide of a binaryOp into its own expression in order to group higher precedence operations
        - Ex: 2+2*2 -> 2+(2*2)

        
