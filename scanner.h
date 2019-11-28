#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

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
}Token_type;

typedef union
{
    Dynamic_string* string;
	int integer; 
	Keyword keyword; 
	double flt; 
} Token_attribute;

typedef struct{
    Token_type type;
    Token_attribute attribute;
}Token;

int free_the_stuff(int retval, Dynamic_string* d_str);

int process_id(Dynamic_string* d_str, Token* token);

int process_int(Dynamic_string* d_str, Token* token);

int process_float(Dynamic_string* d_str, Token* token);

int get_token(Token *token);

#endif