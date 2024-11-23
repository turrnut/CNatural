@echo off
mkdir bin || echo building...
gcc -o bin\emi.exe src\toy.c src\compiler\compiler.c src\compiler\lexer.c src\debug\debug.c src\ir\ir.c src\runtime\runtime.c src\security\memory.c src\types\object.c src\types\table.c src\types\value.c
bin\emi