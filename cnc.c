
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_COMPARISON,
    TOKEN_LOGICAL,
    TOKEN_BOOLEAN,
    TOKEN_STRING,
    TOKEN_UNKNOWN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char value[256];
} Token;

// Lexer function that takes input string and returns an array of tokens
Token* lexer(const char *input, int *tokenCount) {
    const char *cursor = input;
    int capacity = 10;
    int count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));

    while (1) {
        // Skip whitespace
        while (isspace(*cursor)) {
            cursor++;
        }

        // End of input
        if (*cursor == '\0') {
            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, capacity * sizeof(Token));
            }
            tokens[count].type = TOKEN_EOF;
            tokens[count].value[0] = '\0';
            count++;
            break;
        }

        // Allocate more space if necessary
        if (count >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(Token));
        }

        Token token;
        token.value[0] = '\0'; // Initialize token value to an empty string

        // Check for parentheses and braces
        if (*cursor == '(') {
            token.type = TOKEN_LPAREN;
            token.value[0] = '(';
            token.value[1] = '\0';
            cursor++;
        } else if (*cursor == ')') {
            token.type = TOKEN_RPAREN;
            token.value[0] = ')';
            token.value[1] = '\0';
            cursor++;
        } else if (*cursor == '{') {
            token.type = TOKEN_LBRACE;
            token.value[0] = '{';
            token.value[1] = '\0';
            cursor++;
        } else if (*cursor == '}') {
            token.type = TOKEN_RBRACE;
            token.value[0] = '}';
            token.value[1] = '\0';
            cursor++;
        }
        // Check for comparison operators
        else if (strncmp(cursor, "==", 2) == 0 || strncmp(cursor, "!=", 2) == 0 || 
                 strncmp(cursor, ">=", 2) == 0 || strncmp(cursor, "<=", 2) == 0) {
            token.type = TOKEN_COMPARISON;
            strncpy(token.value, cursor, 2);
            token.value[2] = '\0';
            cursor += 2;
        } else if (*cursor == '>' || *cursor == '<') {
            token.type = TOKEN_COMPARISON;
            token.value[0] = *cursor;
            token.value[1] = '\0';
            cursor++;
        }
        // Check for logical operators
        else if (strncmp(cursor, "&&", 2) == 0 || strncmp(cursor, "||", 2) == 0) {
            token.type = TOKEN_LOGICAL;
            strncpy(token.value, cursor, 2);
            token.value[2] = '\0';
            cursor += 2;
        }
        // Check for boolean values
        else if (strncmp(cursor, "true", 4) == 0 && !isalnum(cursor[4])) {
            token.type = TOKEN_BOOLEAN;
            strncpy(token.value, "true", 4);
            token.value[4] = '\0';
            cursor += 4;
        } else if (strncmp(cursor, "false", 5) == 0 && !isalnum(cursor[5])) {
            token.type = TOKEN_BOOLEAN;
            strncpy(token.value, "false", 5);
            token.value[5] = '\0';
            cursor += 5;
        }
        // Check for strings
        else if (*cursor == '"') {
            token.type = TOKEN_STRING;
            int i = 0;
            cursor++; // Skip opening quote
            while (*cursor != '"' && *cursor != '\0') {
                token.value[i++] = *cursor;
                cursor++;
            }
            token.value[i] = '\0';
            if (*cursor == '"') {
                cursor++; // Skip closing quote
            }
        }
        // Check for operators
        else if (strchr("+-*/", *cursor)) {
            token.type = TOKEN_OPERATOR;
            token.value[0] = *cursor;
            token.value[1] = '\0';
            cursor++;
        }
        // Check for numbers
        else if (isdigit(*cursor)) {
            token.type = TOKEN_NUMBER;
            int i = 0;
            while (isdigit(*cursor)) {
                token.value[i++] = *cursor;
                cursor++;
            }
            token.value[i] = '\0';
        }
        // Check for identifiers
        else if (isalpha(*cursor)) {
            token.type = TOKEN_IDENTIFIER;
            int i = 0;
            while (isalnum(*cursor)) {
                token.value[i++] = *cursor;
                cursor++;
            }
            token.value[i] = '\0';
        }
        // Unknown token
        else {
            token.type = TOKEN_UNKNOWN;
            token.value[0] = *cursor;
            token.value[1] = '\0';
            cursor++;
        }

        tokens[count++] = token;
    }

    *tokenCount = count;
    return tokens;
}

// Define the existing string as a dynamically allocated pointer
char *Main = NULL;
char *includesList = NULL;

// Function to append a string to the existing string
void appendMain(const char *stringToAppend) {
    // Calculate the new required length
    size_t newLength = (Main ? strlen(Main) : 0) + strlen(stringToAppend) + 1;

    // Reallocate memory for the new length
    char *temp = realloc(Main, newLength);
    if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        free(Main); // Free existing memory on failure
        Main = NULL;
        return;
    }

    Main = temp;

    // Append the new string
    if (Main[0] == '\0') {
        strcpy(Main, stringToAppend); // Initialize if it's empty
    } else {
        strcat(Main, stringToAppend); // Append otherwise
    }
}


// Function to append a string to the existing string
void appendIncludes(const char *stringToAppend) {
    // Calculate the new required length
    size_t newLength = (includesList ? strlen(includesList) : 0) + strlen(stringToAppend) + 1;

    // Reallocate memory for the new length
    char *temp = realloc(includesList, newLength);
    if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        free(includesList); // Free existing memory on failure
        includesList = NULL;
        return;
    }

    includesList = temp;

    // Append the new string
    if (includesList[0] == '\0') {
        strcpy(includesList, stringToAppend); // Initialize if it's empty
    } else {
        strcat(includesList, stringToAppend); // Append otherwise
    }
}

typedef enum {
    DEFAULT,
    VOID,
    IN_MAIN
} Status;

Status currentStatus;
int braceCnt = 0;

void tok_id(char* value, Token* tokens, int current_idx) {
    if (strcmp(value, "void") == 0) {
        currentStatus = VOID;
        return;
    } else if (strcmp(value, "main") == 0 && currentStatus == VOID) {
        appendMain("int main ");
        currentStatus = IN_MAIN;
        return;
    } else {
        appendMain(value);
    }

    if (currentStatus != IN_MAIN) {
        currentStatus = DEFAULT;
    }

    appendMain(" ");
}

void tok_num(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}
void tok_op(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}
void tok_comp(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}
void tok_logi(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}
void tok_bool(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}
void tok_str(char* value, Token* tokens, int current_idx) {
    appendMain("\"");
    appendMain(value);
    appendMain("\"");
}
void tok_unknown(char* value, Token* tokens, int current_idx) {
    // only accept certain chars?
    // if (
        // strcmp((const char*)value , ";") == 0 ||
        // strcmp((const char*)value , "\n") == 0
        // strcmp((const char*)value , "*") == 0 ||
        // strcmp((const char*)value , "*") == 0 ||
        // strcmp((const char*)value , "*") == 0 ||
        // strcmp((const char*)value , "*") == 0 ||
        // strcmp((const char*)value , "*") == 0 ||
        // strcmp((const char*)value , "*") == 0
    // ){
    // }
    appendMain(value);
}

void tok_lparen(char* value, Token* tokens, int current_idx) {
    if (currentStatus == IN_MAIN && braceCnt == 0) {
        appendMain(value);
        appendMain("int argc, char const *argv[] ");
        return;
    }
    appendMain(value);
}
void tok_rparen(char* value, Token* tokens, int current_idx) {
    appendMain(value);
}

void tok_lbrace(char* value, Token* tokens, int current_idx) {
    braceCnt ++;
    appendMain(value);
}
void tok_rbrace(char* value, Token* tokens, int current_idx) {
    braceCnt --;
    if (currentStatus == IN_MAIN && braceCnt == 0) {
        appendMain("return 0;");
        currentStatus = DEFAULT;
    }
    appendMain(value);
}
void tok_eof(char* value, Token* tokens, int current_idx) {

}

void convertC(Token* tokens, int tokenCount) {
    currentStatus = DEFAULT;
    for (int i = 0; i < tokenCount; i++) {
        switch (tokens[i].type) {
            case TOKEN_IDENTIFIER:
                tok_id(tokens[i].value, tokens, i);
                break;
            case TOKEN_NUMBER:
                tok_num(tokens[i].value, tokens, i);
                break;
            case TOKEN_OPERATOR:
                tok_op(tokens[i].value, tokens, i);
                break;
            case TOKEN_COMPARISON:
                tok_comp(tokens[i].value, tokens, i);
                break;
            case TOKEN_LOGICAL:
                tok_logi(tokens[i].value, tokens, i);
                break;
            case TOKEN_BOOLEAN:
                tok_bool(tokens[i].value, tokens, i);
                break;
            case TOKEN_STRING:
                tok_str(tokens[i].value, tokens, i);
                break;
            case TOKEN_UNKNOWN:
                tok_unknown(tokens[i].value, tokens, i);
                break;
            case TOKEN_LPAREN:
                tok_lparen(tokens[i].value, tokens, i);
                break;
            case TOKEN_RPAREN:
                tok_rparen(tokens[i].value, tokens, i);
                break;
            case TOKEN_LBRACE:
                tok_lbrace(tokens[i].value, tokens, i);
                break;
            case TOKEN_RBRACE:
                tok_rbrace(tokens[i].value, tokens, i);
                break;
            case TOKEN_EOF:
                tok_eof(tokens[i].value, tokens, i);
                break;
        }
    }
}

// Function to read the contents of a file
char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

// Function to write a string to a file
void writeFile(const char *fileName, const char *content) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", fileName);
        return;
    }

    // Write the content to the file
    fprintf(file, "%s", content);

    fclose(file);
}


int main(int argc, char const *argv[]) {
    if (!system(NULL)) {
        printf("Error: Command processor is not available.\n");
        return 1;
    }

    if (argv[2] == NULL) {
        printf("Usage: cnc [c-compiler] [file-name]");
        return 1;
    }

    const char *input = readFile(argv[2]);

    // Initialize the existing string with a starting value
    Main = malloc(1); // Start with an empty string
    includesList = malloc(1); // Start with an empty string
    if (Main == NULL || includesList == NULL) {
        printf("Error: Initial memory allocation failed.\n");
        return 1;
    }
    Main[0] = '\0'; // Null-terminate the empty string
    includesList[0] = '\0'; // Null-terminate the empty string

    int tokenCount;
    Token *tokens = lexer(input, &tokenCount);
    
    convertC(tokens, tokenCount);


    // appendMain("    return 0;\n}");

    appendIncludes("#include <stdio.h>\n#include <string.h>\n#include <ctype.h>\n#include <stdlib.h>\n#include <stddef.h>\n#include <math.h>\n#include <time.h>\n#include <signal.h>\n#include <setjmp.h>\n#include <locale.h>\n#include <stdint.h>\n#include <stdbool.h>\n#include <limits.h>\n#include <float.h>\n#include <inttypes.h>\n#include <errno.h>\n#include <wchar.h>\n#include <wctype.h>\n#include <stdarg.h>\n");

    appendIncludes(Main);

    const char* out = "out.c";

    writeFile(out, includesList);
    // printf(strcat(strcat((char*)argv[1] ," "), out));
    int result = system(strcat(strcat((char*)argv[1] ," "), out));

    if (result == 0){
        remove(out);
    }

    free(Main);
    free(includesList);

    free(tokens);
    return 0;
}
