# DV1457-CompilerBackend
A Compiler backend for the "CALC3" language

## Folder structure
- ./ - Generated files will be places here
- ./bin/ - Compiler binaries
- ./lib/ - Library output folder
- ./lexyacc-code/ - Compiler sourcecode
- ./src/ - Library sourcecode and library test program
- ./tests/ - The test .calc programs

## Makefile
`make` / `make all` - Build all the compilers
`make test` - Run test, see below
`make clean` - Remove built files

### How to test
Run `make test`, this will test both the implementation against each of the
.calc files in the /tests folder, and compare the outputs from the two programs.

## Important sourcecode files
`lexyacc-code/calc3.c.c` - Generates the `bin/calc3.c` program. This program compiles
a .calc program into a .c file.

`lexyacc-code/calc3i.c` - Generates the `bin/calc3i` program. This program compiles
a .calc program into a .S file. (A .S file is a .s file with preprocessor
information.)