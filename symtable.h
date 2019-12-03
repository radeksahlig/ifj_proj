#ifndef _SYMTABLE_H
#define _SYMTABLE_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#include "string.h"
#define TRUE 1
#define FALSE 0

typedef enum {
    nVariable,
    nFunction,
} tNodeType;

typedef struct tBSTNode {
	char* Key;
  	tNodeType nodeType;
	void* content;
	struct tBSTNode * LPtr;
	struct tBSTNode * RPtr;
} *tBSTNodePtr;


void BSTInit   (tBSTNodePtr *);
tBSTNodePtr BSTSearch  (tBSTNodePtr, char*);
int BSTInsert (tBSTNodePtr *, Dynamic_string*, void*, tNodeType);
void BSTDelete (tBSTNodePtr *, char*);
void BSTDispose(tBSTNodePtr *);


typedef struct variable {
    int dataType;
} tInsideVariable;

typedef struct symtable {
    tBSTNodePtr root;
} tSymtable;

typedef struct function {
    int returning;
    bool declared;
    bool defined;
    tSymtable *local;
    int parameters;
    char* paramName[10];
} tInsideFunction;



//----------------------------------


void symtableInit(tSymtable*);
int symtableInsertF(tSymtable*, Dynamic_string*);
int symtableInsertV(tSymtable*, Dynamic_string*);
tBSTNodePtr symtableSearch(tSymtable*, char*);
void symtableDelete(tSymtable*, char*);
void symtableDispose(tSymtable*);
#endif
