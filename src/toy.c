//> Chunks of Bytecode main-c
//> Scanning on Demand main-includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

//< Scanning on Demand main-includes
#include "include/common.h"
//> main-include-chunk
#include "ir/ir.h"
//< main-include-chunk
//> main-include-debug
#include "debug/debug.h"
//< main-include-debug
//> A Virtual Machine main-include-vm
#include "runtime/runtime.h"

const char* LANGUAGE_NAME = "Emi";
const char* LANGUAGE_EXECUTABLE = "emi";
const char* LANGUAGE_VERSION = "v1.0";
const char* LANGUAGE_YEAR = "2024";

static void help() {
  printf("The %s programming language %s\nCopyright (c) %s Turrnut\n", LANGUAGE_NAME, LANGUAGE_VERSION, LANGUAGE_YEAR);
  printf("Usage:\n");
  printf("  %s\n    to open the REPL\n", LANGUAGE_EXECUTABLE);
  printf("  %s [path]\n    to execute a file\n", LANGUAGE_EXECUTABLE);
  printf("  %s --help\n", LANGUAGE_EXECUTABLE);
  printf("  %s -h\n    to display this message\n", LANGUAGE_EXECUTABLE);
  printf("  %s --init\n");
  printf("  %s -i\n    to initialize a project\n\n", LANGUAGE_EXECUTABLE);
}

static int init(){
    FILE *emiFile = fopen("main.emi", "w");
    if (emiFile == NULL) {
        perror("Error creating main.emi");
        return 1;
    }
    fprintf(emiFile, "# main.emi\n");
    fprintf(emiFile, "println \"Hello, World!\";\n");
    fclose(emiFile);
    const char *username = "YourName";

    // Create and write to config.json
    FILE *configFile = fopen("config.json", "w");
    if (configFile == NULL) {
        perror("Error creating config.json");
        return 1;
    }
    fprintf(configFile, "{\n");
    fprintf(configFile, "  \"name\": \"myProgram\",\n");
    fprintf(configFile, "  \"author\": \"%s\",\n", username);
    fprintf(configFile, "  \"version\": \"v1.0\"\n");
    fprintf(configFile, "}\n");
    fclose(configFile);

    printf("Project initialized successfully.\n");
}

static char *trim(char *str) {
    char *end;

    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;

    // If the string is empty
    if (*str == '\0') return str;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null-terminate the string
    *(end + 1) = '\0';

    return str;
}
static void repl() {
  char line[1024];
  for (;;) {
    printf("$ ");
    
    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }
    
    if (strcmp(trim(line), "exit") == 0) {
      break;
    }

    interpret(line);
  }
}
//< Scanning on Demand repl
//> Scanning on Demand read-file
static char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");
//> no-file
  if (file == NULL) {
    fprintf(stderr, "FATAL: Could not open file \"%s\".\n", path);
    exit(74);
  }
//< no-file

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
//> no-buffer
  if (buffer == NULL) {
    fprintf(stderr, "FATAL: Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  
//< no-buffer
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
//> no-read
  if (bytesRead < fileSize) {
    fprintf(stderr, "FATAL: Could not read file \"%s\".\n", path);
    exit(74);
  }
  
//< no-read
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}
//< Scanning on Demand read-file
//> Scanning on Demand run-file
static void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source); // [owner]

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}
//< Scanning on Demand run-file

int main(int argc, const char* argv[]) {
//> A Virtual Machine main-init-vm
  initVM();
  
  if (argc == 1) {
    printf("The %s programming language %s REPL\nCopyright (c) %s Turrnut\n\n", LANGUAGE_NAME, LANGUAGE_VERSION, LANGUAGE_YEAR);
    repl();
  } else if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      help();
    } else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--init") == 0) {
      return init();
    } else {
      runFile(argv[1]);
    }
  } else {
    fprintf(stderr, "Error: Wrong number of arguments provided.\n\n");
    help();
    exit(64);
  }
  
  freeVM();
//< Scanning on Demand args
/* A Virtual Machine main-free-vm < Scanning on Demand args
  freeVM();
*/
/* Chunks of Bytecode main-chunk < Scanning on Demand args
  freeChunk(&chunk);
*/
  return 0;
}