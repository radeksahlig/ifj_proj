/*----------------------------------------------------
********************NOTES*****************************
TODO INDENT/DEDENT pomocni zasobnik
----------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "error.h"
#include "scanner.h"

//Automat states
#define START_STATE     100 
#define IDKW_STATE      102
#define NUMBER_STATE    103
#define STRING_STATE    104
#define QUOTE_STATE     105
#define DIV_STATE       106
#define DOC_STRING_STATE    107
#define MORE_STATE      110
#define LESS_STATE      111
#define EQ_STATE        112
#define NEQ_STATE       113
#define DECIMAL_BEGIN_STATE   114
#define DECIMAL_STATE   115
#define ESCAPE_CHAR_STATE   116
#define EXPONENT_E      117
#define EXPONENT_NUMBER 118
#define EXPONENT_SIGN   119
#define DENT_COUNTING   120

FILE *file;
int indentation_stack[64];
indentation_stack[0] = 0;
bool in_indentation = false;

int get_token(Token *token){

    int scanner_state = START_STATE;
    int quote_count = 0;
    int indentation_count = 0;
    char c;
    token->type = TOKEN_EMPTY_FILE;

    while (true){
        
        c = (char) getc(file);

        switch(scanner_state){
            
            case(START_STATE):
                
                if(isspace(c) && c != ' '){
                    scanner_state = START_STATE;
                }

                else if(c == ' '){
                    indentation_count++;
                    in_indentation = true;
                    scanner_state = DENT_COUNTING;
                }

                else if (c == '\n'){
                    token->type = TOKEN_EOL;
                    indentation_count = 0;
                    return SCANNER_OK;
                }

                else if (c == EOF){
                    token->type = TOKEN_EOF;
                    return SCANNER_OK;
                }
                
                else if (c == '_' || isalpha(c)){
                    //TODO save char to string
                    scanner_state = IDKW_STATE;
                }

                else if (isdigit(c)){
                    //TODO save digit to string
                    scanner_state = NUMBER_STATE;
                }

                else if (c == '\''){
                    scanner_state = STRING_STATE;
                }

                else if (c == '\"'){
                    scanner_state = QUOTE_STATE;
                    quote_count++;
                }

                else if (c == '+'){
                    token->type = TOKEN_PLUS;
                    return SCANNER_OK;
                }

                else if (c == '-'){
                    token->type = TOKEN_MINUS;
                    return SCANNER_OK;
                }

                else if (c == '*'){
                    token->type = TOKEN_MUL;
                    return SCANNER_OK;
                }

                else if (c == '/'){
                    scanner_state = DIV_STATE; // '/' -> float, '//' -> integer, expects integer operands
                }

                else if (c == '>'){
                    scanner_state = MORE_STATE;
                }

                else if (c == '<'){
                    scanner_state = LESS_STATE;
                }

                else if (c == '='){
                    scanner_state = EQ_STATE;
                }

                else if (c == '!'){
                    scanner_state = NEQ_STATE;
                }

                else if (c == ','){
                    token->type = TOKEN_COMMA;
                    return SCANNER_OK;
                }

                else if (c == '('){
                    token->type = TOKEN_L_BRACKET;
                    return SCANNER_OK;
                }

                break;

            case (DENT_COUNTING):
                if (c == ' '){
                    indentation_count++;
                }
                else if(isspace(c) && c != ' '){
                    scanner_state = DENT_COUNTING;
                }
                else if(c == '\"')
                    scanner_state = QUOTE_STATE;
                else{
                    //TODO process dent_stack
                }
                break;

            case (IDKW_STATE):
                if(isalnum(c) || c == '_'){ //next char is alphanumeric or '_'
                    //TODO save char to string
                }
                else{
                    /*TODO IDKW ended, process it
                    in case of keyword match:   token->type = TOKEN_KEYWORD
                    other cases:                token->type = TOKEN_ID
                    */
                }

                break;
            
            case (NUMBER_STATE):
                if(isdigit(c)){
                    //TODO store the current digit
                }
                else if(c == '.'){
                    scanner_state = DECIMAL_BEGIN_STATE;
                    //TODO store the point
                }
                else if(tolower(c) == 'e'){
                    scanner_state = EXPONENT_E;
                    //TODO save 'e' to string
                } 
                else{
                    //TODO number ended, process it
                }

                break;

            case (DECIMAL_BEGIN_STATE):
                if(isdigit(c)){
                    scanner_state = DECIMAL_STATE;
                    //TODO store the current digit
                }
                else{
                    return SCANNER_ERROR;
                }

                break;

            case (DECIMAL_STATE):
                if(isdigit(c)){
                    scanner_state = DECIMAL_STATE; //stay in this state, might be unnecessary
                    //TODO store the digit
                }
                else if (tolower(c) == 'e'){
                    scanner_state = EXPONENT_E;
                    //TODO save the 'e' to the string
                }
                else{
                    //TODO number ended, store it
                    token->type = TOKEN_FLOAT;
                }
                
                break;
            
            case (EXPONENT_E):
                if(isdigit(c)){
                    scanner_state = EXPONENT_NUMBER;
                    //TODO store the number 
                }
                else if(c == '+' || c == '-'){
                    scanner_state = EXPONENT_SIGN;
                    //TODO save the sign to the string
                }
                else{
                    return SCANNER_ERROR;
                }
                
                break;
            
            case (EXPONENT_SIGN):
                if(isdigit(c)){
                    scanner_state = EXPONENT_NUMBER;
                    //TODO store the number
                }
                else{
                    return SCANNER_ERROR;
                }
                break;

            case (EXPONENT_NUMBER):
                if(isdigit(c)){
                    //TODO store the number
                }
                else{
                    //TODO number ended, process it
                    token->type = TOKEN_FLOAT;
                }
                break;

            case (STRING_STATE):
                if(c == '\''){ //string ended
                    token->type = TOKEN_STRING;
                    //TODO store the "'"
                    return SCANNER_OK;
                }
                else if(c == "\\"){
                    scanner_state = ESCAPE_CHAR_STATE; // for ’ \" ’, ’ \' ’, ’ \n ’, ’ \t ’, ’ \\ ’, others can be written directly
                }
                else if(c <= 31){
                    return SCANNER_ERROR;
                }
                else{ //anything else simply belongs to the string
                    //TODO store the string
                }

                break;

            case (ESCAPE_CHAR_STATE):
                if(c == '"'){
                    c = '"';
                    //TODO store the char
                }
                else if(c == '\''){
                    c = '\'';
                    //TODO store the char
                }
                else if(c == 'n'){
                    c = '\n';
                    //TODO store the char
                }
                else if(c == 't'){
                    c = '\t';
                    //TODO store the char
                }
                else if(c == '\\'){
                    c = '\\';
                    //TODO store the char
                }
                //TODO hexadecimal escape sequence '\xhh', where hh is a two digit hexadecimal number from 00 to FF ... other scanner states needed, cca +200 lines
                
                break;
            

            case (QUOTE_STATE):
                if (quote_count == 3){
                    quote_count = 0;
                    scanner_state = DOC_STRING_STATE;
                }
                else if (c == '\"'){
                    quote_count++;
                    scanner_state = QUOTE_STATE;
                }
                else{
                    return SCANNER_ERROR;
                }   

                break;

            case(DOC_STRING_STATE):
                if(quote_count == 3){
                    scanner_state = START_STATE;
                }
                else if (c != '\"' && quote_count > 0){
                    quote_count = 0;
                }
                else if (c == '\"'){
                    quote_count++;
                }
                else if (c == EOF){
                    return SCANNER_ERROR;
                }
                
                break;

            case (DIV_STATE):
                /*  in case of '/' : token->type = TOKEN_FLOAT_DIV;
                    in case of '//': token->type = TOKEN_INT_DIV;
                */


            case (MORE_STATE):
                if (c == '='){
                    token->type = TOKEN_MEQ;
                }
                else{
                    token->type = TOKEN_MORE;
                }
                return SCANNER_OK;
                
            
            case (LESS_STATE):
                if (c == '='){
                    token->type = TOKEN_LEQ;
                }
                else{
                    token->type = TOKEN_LESS;
                }
                return SCANNER_OK;

            case (EQ_STATE):
                if (c == '='){
                    token->type = TOKEN_EQ;
                }
                else{
                    token->type = TOKEN_ASSIGN;
                }
                return SCANNER_OK;

            case (NEQ_STATE):
                if (c == '='){
                    token->type = TOKEN_NEQ;
                    return SCANNER_OK;
                }
                else{
                    return SCANNER_ERROR;
                }
            
        }
    }

}