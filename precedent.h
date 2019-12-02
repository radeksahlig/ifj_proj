#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symstack.h"
#include "error.h"


int idkfunkce(symStack *stack, Token *help);

int precedent_analys(Token *help);

int reduction(symStack *stack);

#endif
