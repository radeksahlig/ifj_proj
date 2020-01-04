/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  string.h                                         *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H


#define INIT_SIZE 10

typedef struct {
    char* string;
    int length;
    int size;
}Dynamic_string;

bool d_string_init(Dynamic_string* str);

bool d_string_add_char(Dynamic_string* str, char c);

bool d_string_add_string(Dynamic_string* d_str, Dynamic_string* attribute_str);

#endif
