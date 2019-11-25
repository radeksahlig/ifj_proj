#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

#include "dynamic_string.h" //TODO

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
    TOKEN_ASSIGN, // =
    TOKEN_NEQ, // !=
}Token_type;

typedef union
{
    //TODO special string
	int integer; 
	Keyword keyword; 
	double decimal; 
} Token_attribute;

typedef struct{
    Token_type type;
    Token_attribute attribute;
}Token

int get_token(Token *token)

#endif
