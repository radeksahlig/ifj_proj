/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  error.h                                          *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#ifndef _ERROR_H
#define _ERROR_H

#define SCANNER_OK      0
#define SYNTAX_OK       0
#define SCANNER_ERROR   1   
#define SYNTAX_ERROR    2
#define SEM_ERROR_DEF 	3
#define SEM_ERROR_TYPE  4
#define SEM_ERROR_PARAM 5
#define SEM_ERROR_OTHER 6
#define ZERO_DIVISION   9
#define INTERNAL_ERROR  99

#endif
