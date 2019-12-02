#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#define TRUE 1
#define FALSE 0

typedef enum {
    nVariable,
    nFunction,
} tNodeType;

typedef struct tBSTNode {
	char* Key;
  tNodeType nodeType;
	void* BSTNodeCont;
	struct tBSTNode * LPtr;
	struct tBSTNode * RPtr;
} *tBSTNodePtr;


void BSTInit   (tBSTNodePtr *);
tBSTNodePtr BSTSearch  (tBSTNodePtr, char*);
void BSTInsert (tBSTNodePtr *, char*, void*, tNodeType);
void BSTDelete (tBSTNodePtr *, char*);
void BSTDispose(tBSTNodePtr *);


typedef struct variable {
    int dataType;
} tInsideVariable;

typedef struct function {
    int returning;
    bool declared;
    bool defined;
    char* parameters;
    char* paramName[10];
} tInsideFunction;

typedef struct symtable {
    tBSTNodePtr root;
} tSymtable;


//----------------------------------


void symtableInit(tSymtable*);
void symtableInsertF(tSymtable*, char*, tInsideFunction*);
void symtableInsertV(tSymtable*, char*, tInsideVariable*);
tBSTNodePtr symtableSearch(tSymtable*, char*);
void symtableDelete(tSymtable*, char*);
void symtableDispose(tSymtable*);

#endif
