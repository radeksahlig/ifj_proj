#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include <stdbool.h>

#include "string.h"
#include "error.h"

typedef enum{
    KEYWORD_DEF,
    KEYWORD_ELSE,
    KEYWORD_IF,
    KEYWORD_NONE,
    KEYWORD_PASS,
    KEYWORD_RETURN,
    KEYWORD_WHILE
}Keyword;

typedef enum{
    TOKEN_EMPTY_FILE,
    TOKEN_KEYWORD,
    TOKEN_ID,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_EOF,
    TOKEN_EOL, 
    TOKEN_COMMA, // ,
    TOKEN_L_BRACKET, // (
    TOKEN_R_BRACKET, // )
    TOKEN_PLUS, // +
    TOKEN_MINUS, // -
    TOKEN_MUL, // *
    TOKEN_FLOAT_DIV, // /
    TOKEN_INT_DIV, // //
    TOKEN_MEQ, // >=
    TOKEN_MORE, // >
    TOKEN_LEQ, // <=
    TOKEN_LESS, //<
    TOKEN_EQ, // ==
    TOKEN_COLON, // :
    TOKEN_ASSIGN, // =
    TOKEN_NEQ, // !=
    TOKEN_INDENT,
    TOKEN_DEDENT,
    TOKEN_PREC_OPEN,
    TOKEN_PREC_CLOSE,
    TOKEN_PREC_ID
    TOKEN_PREC_FLOAT
    TOKEN_PREC_INTEGER
    TOKEN_PREC_STRING
}Token_type;

typedef union{
    Dynamic_string* string;
	int integer; 
	Keyword keyword; 
	double flt; 
} Token_attribute;

typedef struct{
    Token_type type;
    Token_attribute attribute;
}Token;

typedef struct stack_item{
    int value;
    struct stack_item* next; 
}  Simple_stack_item;


typedef struct{
    Simple_stack_item* top;
} Simple_stack;

int free_the_stuff(int retval, Dynamic_string* d_str);

int process_id(Dynamic_string* d_str, Token* token);

int process_int(Dynamic_string* d_str, Token* token);

int process_float(Dynamic_string* d_str, Token* token);

void set_stack(Simple_stack* stack);

void set_file(FILE* file);

void set_d_string(Dynamic_string* str);

int get_token(Token *token);

#endif
