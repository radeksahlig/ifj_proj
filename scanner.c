/*----------------------------------------------------
********************NOTES*****************************
TODO implement the indentation stack.
----------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

//Automat states
#define START_STATE         100 
#define IDKW_STATE          102 
#define NUMBER_STATE        103
#define STRING_STATE        104
#define FIRST_QUOTE_BEG     105
#define DIV_STATE           106
#define DOC_STRING_STATE    107
#define MORE_STATE          110
#define LESS_STATE          111
#define EQ_STATE            112
#define NEQ_STATE           113
#define DECIMAL_BEGIN_STATE 114
#define DECIMAL_STATE       115
#define ESCAPE_CHAR_STATE   116
#define EXPONENT_E          117
#define EXPONENT_NUMBER     118
#define EXPONENT_SIGN       119
#define INDENTATION_COUNTING   120
#define INDENT_STATE        121
#define DEDENT_STATE        122
#define HEXA_FIRST_STATE    123
#define HEXA_SECOND_STATE   124
#define INT_DIV_STATE       126
#define SECOND_QUOTE_BEG    127
#define THIRD_QUOTE_BEG     128
#define SECOND_QUOTE_END    129
#define THIRD_QUOTE_END     130
#define FIRST_QUOTE_END     131
#define LINE_COMMENT        132



FILE *file;
Dynamic_string* dynamic_str;

int ind_stack[64];
bool in_indentation = false;
int top = 0;

//stack functions
int stack_pop(int stack[]) {
    int num;
    num = stack[top--];
    return num;
}

void stack_push(int stack[], int num){
    stack[++top] = num;
}

int free_the_stuff(int retval, Dynamic_string* d_str){
    free(d_str->string);
    return retval;
}

int process_id(Dynamic_string* d_str, Token* token){

    if (!strcmp(d_str->string, "def"))
        token->attribute.keyword = KEYWORD_DEF;

    else if (!strcmp(d_str->string, "else"))
        token->attribute.keyword = KEYWORD_ELSE;
    
    else if (!strcmp(d_str->string, "if"))
        token->attribute.keyword = KEYWORD_IF;

    else if (!strcmp(d_str->string, "None"))
        token->attribute.keyword = KEYWORD_NONE;

    else if (!strcmp(d_str->string, "pass"))
        token->attribute.keyword = KEYWORD_PASS;

    else if (!strcmp(d_str->string, "return"))
        token->attribute.keyword = KEYWORD_RETURN;

    else if (!strcmp(d_str->string, "while"))
        token->attribute.keyword = KEYWORD_WHILE;

    else{
        token->type = TOKEN_ID;
        if (!d_string_add_string(d_str, token->attribute.string)){
            return free_the_stuff(INTERNAL_ERROR, d_str);
        }
    }

    token->type = TOKEN_KEYWORD;
    return free_the_stuff(SCANNER_OK, d_str);


    
}

int process_int(Dynamic_string* d_str, Token* token){
    char *endptr;
    int integer = strtol(d_str->string, &endptr, 10);
    token->type = TOKEN_INTEGER;
    token->attribute.integer = integer;
    return free_the_stuff(SCANNER_OK, d_str);
}

int process_float(Dynamic_string* d_str, Token* token){
    char *endptr;
    double flt = strtod(d_str->string, &endptr);
    token->type = TOKEN_FLOAT;
    token->attribute.flt = flt;
    return free_the_stuff(SCANNER_OK, d_str);
}

int get_token(Token *token){
    

    if(file == NULL || dynamic_str == NULL){
        return INTERNAL_ERROR;        
    }

    token->attribute.string = dynamic_str;


    ind_stack[top] = 0;
    int scanner_state = START_STATE;
    int indentation_count = 0;
    char c;
    char hexa[2] = { 0 };
    
    Dynamic_string string;
	Dynamic_string *str = &string;

    if(!d_string_init(str)){
        return INTERNAL_ERROR;
    }
    
    token->type = TOKEN_EMPTY_FILE;

    while (true){
        
        c = (char) getc(file);

        switch(scanner_state){
            
            case(START_STATE):
                
                if(isspace(c) && c != ' '){
                    scanner_state = START_STATE;
                }
                //TODO still that goddamn stack >:(
                else if(c == ' '){
                    
                    if(ind_stack[top] < ++indentation_count){
                        stack_push(ind_stack, indentation_count);
                    }
                    
                    in_indentation = true;
                    scanner_state = INDENTATION_COUNTING;
                }

                else if (c == '\n'){
                    token->type = TOKEN_EOL;
                    indentation_count = 0;
                    return free_the_stuff(SCANNER_OK, str);
                }

                else if (c == EOF){
                    token->type = TOKEN_EOF;
                    return free_the_stuff(SCANNER_OK, str);
                }
                
                else if (c == '_' || isalpha(c)){
                    
                    if(!d_string_add_char(str, (char) c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }

                    scanner_state = IDKW_STATE;
                }

                else if (isdigit(c)){
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }

                    scanner_state = NUMBER_STATE;
                }

                else if (c == '#'){
                    scanner_state = LINE_COMMENT;
                }
                
                else if (c == '\''){
                    scanner_state = STRING_STATE;
                }

                else if (c == '\"'){
                    scanner_state = SECOND_QUOTE_BEG;
                }

                else if(c == ':'){
                    token->type = TOKEN_COLON;
                }

                else if (c == '+'){
                    token->type = TOKEN_PLUS;
                    return free_the_stuff(SCANNER_OK, str);
                }

                else if (c == '-'){
                    token->type = TOKEN_MINUS;
                    return free_the_stuff(SCANNER_OK, str);
                }

                else if (c == '*'){
                    token->type = TOKEN_MUL;
                    return free_the_stuff(SCANNER_OK, str);
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
                    return free_the_stuff(SCANNER_OK, str);
                }

                else if (c == '('){
                    token->type = TOKEN_L_BRACKET;
                    return free_the_stuff(SCANNER_OK, str);
                }

                break;

            case (INDENTATION_COUNTING):
                if (c == ' '){
                    
                    if(ind_stack[top] < ++indentation_count){
                        stack_push(ind_stack, indentation_count);
                    }
                }
                else if(isspace(c) && c != ' '){
                    scanner_state = INDENTATION_COUNTING;
                }
                else if(c == '\"')
                    scanner_state = SECOND_QUOTE_BEG;
                else{
                    //TODO indentation ended, process it
                    if(indentation_count < ind_stack[top]){
                        scanner_state = DEDENT_STATE;
                    }
                    scanner_state = START_STATE; 
                }
                break;

            case (IDKW_STATE):
                if(isalnum(c) || c == '_'){ //next char is alphanumeric or '_'
                     
                     if(!d_string_add_char(str, (char) c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    /*IDKW ended, process it
                    in case of keyword match:   token->type = TOKEN_KEYWORD
                    other cases:                token->type = TOKEN_ID
                    */
                   ungetc(c,file);
                   return process_id(str, token);
                }

                break;
            
            case (NUMBER_STATE):
                if(isdigit(c)){
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else if(c == '.'){
                    scanner_state = DECIMAL_BEGIN_STATE;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else if(tolower(c) == 'e'){
                    scanner_state = EXPONENT_E;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                } 
                else{
                    ungetc(c,file);
                    return process_int(str, token);
                }

                break;

            case (DECIMAL_BEGIN_STATE):
                if(isdigit(c)){
                    scanner_state = DECIMAL_STATE;
                   
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }

                break;

            case (DECIMAL_STATE):
                if(isdigit(c)){
                    scanner_state = DECIMAL_STATE; //stay in this state, might be unnecessary
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else if (tolower(c) == 'e'){
                    scanner_state = EXPONENT_E;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    ungetc(c, file);
                    return process_float(str, token);
                }
                
                break;
            
            case (EXPONENT_E):
                if(isdigit(c)){
                    scanner_state = EXPONENT_NUMBER;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else if(c == '+' || c == '-'){
                    scanner_state = EXPONENT_SIGN;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }
                
                break;
            
            case (EXPONENT_SIGN):
                if(isdigit(c)){
                    scanner_state = EXPONENT_NUMBER;
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }
                break;

            case (EXPONENT_NUMBER):
                if(isdigit(c)){
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    ungetc(c,file);
                    return process_float(str, token);
                }
                break;

            case (STRING_STATE):
                if(c == '\''){ //string ended
                    
                    if(!d_string_add_string(str, token->attribute.string)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }

                    token->type = TOKEN_STRING;
                }
                else if(c == '\\'){
                    scanner_state = ESCAPE_CHAR_STATE; // for ’ \" ’, ’ \' ’, ’ \n ’, ’ \t ’, ’ \\ ’, others can be written directly
                }
                else if(c <= 31){
                    return free_the_stuff(SCANNER_ERROR, str);
                }
                else{ //anything else simply belongs to the string
                    
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }

                break;

            case (ESCAPE_CHAR_STATE):
                
                if(c <= 31){
                    return free_the_stuff(SCANNER_ERROR, str);
                }
                if(c == '"'){
                    c = '"';
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    scanner_state = STRING_STATE;
                }
                else if(c == '\''){
                    c = '\'';
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    scanner_state = STRING_STATE;
                }
                else if(c == 'n'){
                    c = '\n';
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    scanner_state = STRING_STATE;
                }
                else if(c == 't'){
                    c = '\t';
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    scanner_state = STRING_STATE;
                }
                else if(c == '\\'){
                    c = '\\';
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    scanner_state = STRING_STATE;
                }
                //hexadecimal escape sequence '\xhh', where hh is a two digit hexadecimal number from 00 to FF
                else if(c == 'x'){
                    scanner_state = HEXA_FIRST_STATE;
                }
                else if(isalpha(c) || isdigit(c)){
                    if(!d_string_add_char(str, '\\')){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    if(!d_string_add_char(str, c)){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }
                
                break;

            case (HEXA_FIRST_STATE):
                if(isdigit(c) || tolower(c) == 'a' || tolower(c) == 'b' || tolower(c) == 'c' || tolower(c) == 'd' || tolower(c) == 'e' || tolower(c) == 'f'){
                    hexa[0] = (char) c;
                    scanner_state = HEXA_SECOND_STATE;
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }

                break;

            case (HEXA_SECOND_STATE):
                if(isdigit(c) || tolower(c) == 'a' || tolower(c) == 'b' || tolower(c) == 'c' || tolower(c) == 'd' || tolower(c) == 'e' || tolower(c) == 'f'){
                    hexa[1] = (char) c;
                    char* endptr;
                    int conv = strtol(hexa, &endptr, 16);
                    c = (char) conv;
                    
                    if(!d_string_add_char(str, 'c')){
                        return free_the_stuff(INTERNAL_ERROR, str);
                    }
                    
                    scanner_state = STRING_STATE;
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }

                break;

            case(LINE_COMMENT):
                if(c == EOL){
                    scanner_state = START_STATE;
                }
                
                break;

            case (SECOND_QUOTE_BEG):
                if(c == '"'){
                    scanner_state = THIRD_QUOTE_BEG;
                }
                else{
                    free_the_stuff(SCANNER_ERROR,str);
                }

                break;

            case (THIRD_QUOTE_BEG):
                if(c == '"'){
                    scanner_state = DOC_STRING_STATE;
                }
                else{
                    free_the_stuff(SCANNER_ERROR,str);
                }

                break;

            case(DOC_STRING_STATE):
                if(c == '"'){
                    scanner_state = SECOND_QUOTE_END;
                }
                else{
                    scanner_state = DOC_STRING_STATE;
                }
                               
                break;


            case (SECOND_QUOTE_END):
                if(c == '"'){
                    scanner_state = THIRD_QUOTE_END;
                }
                else{
                    scanner_state = DOC_STRING_STATE;
                }

                break;

            case (THIRD_QUOTE_END):
                if(c == '"'){
                    scanner_state = START_STATE;
                }
                else{
                    scanner_state = DOC_STRING_STATE;
                }

                break;

            case (DIV_STATE):
                /*  in case of '/' : token->type = TOKEN_FLOAT_DIV;
                    in case of '//': token->type = TOKEN_INT_DIV;
                */
               if(c != '/'){
                   ungetc(c, file);
                   token->type = TOKEN_FLOAT_DIV;
                   return free_the_stuff(SCANNER_OK, str);
               }
               else if (c == '/'){
                   token->type = TOKEN_INT_DIV;
                   return free_the_stuff(SCANNER_OK, str);
               }
               break;


            case (MORE_STATE):
                if (c == '='){
                    token->type = TOKEN_MEQ;
                    return free_the_stuff(SCANNER_OK, str);
                }
                else{
                    ungetc(c, file);
                    token->type = TOKEN_MORE;
                    return free_the_stuff(SCANNER_OK, str);
                }

            
            case (LESS_STATE):
                if (c == '='){
                    token->type = TOKEN_LEQ;
                    return free_the_stuff(SCANNER_OK, str);
                }
                else{
                    ungetc(c, file);
                    token->type = TOKEN_LESS;
                    return free_the_stuff(SCANNER_OK, str);
                }

            case (EQ_STATE):
                if (c == '='){
                    token->type = TOKEN_EQ;
                    return free_the_stuff(SCANNER_OK, str);
                }
                else{
                    token->type = TOKEN_ASSIGN;
                    return free_the_stuff(SCANNER_OK, str);
                }

            case (NEQ_STATE):
                if (c == '='){
                    token->type = TOKEN_NEQ;
                    return free_the_stuff(SCANNER_OK, str);
                }
                else{
                    return free_the_stuff(SCANNER_ERROR, str);
                }
            
        }
    }

}

int main(){
    return 0;
}