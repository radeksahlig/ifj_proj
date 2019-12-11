#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "string.h"
#include "error.h"

bool d_string_init(Dynamic_string* d_str){
    d_str->string = (char*) malloc(INIT_SIZE);
    
    if(d_str->string == NULL){
        return false;
    } 
   
    d_str->length = 0;
    d_str->string[0] = '\0';
    d_str->size = INIT_SIZE;

    return true;
}

bool d_string_add_char(Dynamic_string* d_str, char c){
    if(d_str->length + 1 > d_str->size){
        d_str->string = (char*) realloc(d_str->string, d_str->size + INIT_SIZE);
        
        if(d_str->string == NULL){
            return false;
        }

        d_str->size += INIT_SIZE;
    }

    d_str->string[d_str->length++] = c;
    d_str->string[d_str->length] = '\0';

    return true;
}

bool d_string_add_string(Dynamic_string* d_str, Dynamic_string* attribute_str){
    if(d_str->length >= attribute_str->size){
        attribute_str->string = (char*) realloc(attribute_str->string, d_str->length + 1);
        
        if(attribute_str->string == NULL){
            return false;
        }
        attribute_str->size = d_str->length + 1;
    }
    strcpy(attribute_str->string, d_str->string);
    attribute_str->length = d_str->length;
    return true;
}

