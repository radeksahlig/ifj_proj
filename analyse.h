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

static int value();

static int arg(int* params, bool sub, int* count);

static int arg_n(int* params, bool sub, int* count);

int initVestFunctions();

int insertVestFunction(char* s, int leng, int pocet);

void initVariable(tInsideVariable *content);

#endif
