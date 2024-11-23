//> IRs of Bytecode debug-h
#ifndef debug_h
#define debug_h

#include "../ir/ir.h"

void disassembleIR(IR* chunk, const char* name);
int disassembleInstruction(IR* chunk, int offset);

#endif
