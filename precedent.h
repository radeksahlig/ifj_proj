#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "symstack.h"
#include "error.h"
#include "string.h"

int precedent_analys(Token* help); //TODO TOKNE TYPE

int idkfunkce(symStack *stack, Token* token); //TODO TOKNE TYPE

Token_type StackTopTerm (symStack *stack);

int stackPushOpen(symStack *stack); 

symStackItem* stackPosition(symStack *stack, int j);

int reduction(symStack *stack); //TODO TOKNE TYPE

void setGlobalVariables(Token* tok, tSymtable* glob);

void setLocTable(tSymtable* local);

void setInFunction(bool in);

#endif
