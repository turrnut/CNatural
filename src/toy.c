/**
 * Author: turrnut
 * Copyrighted © turrnut under the Apache 2.0 license
 *
 * We hoped that you will use this piece of open source
 * software fairly. read the LICENSE for more details about
 * how you can use it, you have freedom to distribute and
 * use this code in your project. However, you will have to
 * state changes you made and include the orginal author of
 * this code.
 *
 * toy.c
 * This is the entry point of the program, which contains the
 * main function.
 *
 */

#include "base/base.h"
#include "ir/ir.h"
#include "test/test.h"
#include "runtime/runtime.h"
#include "utils/files.h"
#include "code/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.0.0"

void version() {
    printf("Version %s\n", VERSION);
}

void help(){
    printf("Tara programming language v%s\nCopyright (c) turrnut under the Apache 2.0 License\n\nOptions:\n\t-h or --help: display the help message\n\t-v or --version: get the current version\n\t-i or --init: initialize a new project\n\nTo run a file:tara <file>\n", VERSION);
}

int run(const char *filename)
{
    start_runtime_environment();
    char* text = readFile(filename);
    
    Result result = execute(filename,text);
    free(text);

    end_runtime_environment();
    if (result == COMPILE_ERROR)
        return 1;
    if (result == RUNTIME_ERROR)
        return 1;
    return 0;
}

int main(int argc, char const *argv[])
{
    switch (argc)
    {
    case 1:
        printf("ERR: No input file(s). Use the -h option to learn more.\n");
        return 1;
    
    case 2:
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0){
            version();
            return 0;
        }

        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            help();
            return 0;
        }
        if (strcmp(argv[1],"--init") == 0 || strcmp(argv[1], "-i") == 0) {
            initProject();
            return 0;
        }
        run(argv[1]);
        break;
    default:{

        
        int i = 1;
        while (i < argc)
        {
            
            run(argv[i]);
            i++;
        }
        break;
        
    }
        
    }
    return 0;
}