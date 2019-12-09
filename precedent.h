#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "symstack.h"
#include "error.h"


int precedent_analys(tSymtable* table); //TODO TOKNE TYPE

int idkfunkce(symStack *stack, Token *help, tSymtable* table); //TODO TOKNE TYPE

Token_type StackTopTerm (symStack *stack);

int stackPushOpen(symStack *stack); 

Token_type stackPosition(symStack *stack, int j);

int reduction(symStack *stack); //TODO TOKNE TYPE


#endif
