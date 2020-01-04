/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  analyse.h                                        *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#ifndef _ANALYSE_H
#define _ANALYSE_H

#include <stdlib.h>

#include "error.h"
#include "scanner.h"
#include "precedent.h"


static int prog();

static int stmt(int prev_indent, int cur_indent);

static int params();

static int param_n();

static int def();

static int rovn(Token* token, char* lastid);

static int value(int count, char* fce);

static int arg(int* params, bool sub, int* count, char* fce);

static int arg_n(int* params, bool sub, int* count, char* fce);

int initVestFunctions();

int insertVestFunction(char* s, int leng, int pocet);

void initVariable(tInsideVariable *content);

void genVestFunctions();

#endif
