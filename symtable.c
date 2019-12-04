

#include "symtable.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void BSTInit (tBSTNodePtr *RootPtr) {
//Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
	 *RootPtr = NULL;
}

tBSTNodePtr BSTSearch (tBSTNodePtr RootPtr, char* K)	{
//Funkce vyhledá uzel v BVS s klíčem K.
	 if(RootPtr != NULL){
		//printf("Hledám ve stromě --- Hledám : %s ... Root : %s\n", K, RootPtr->Key);
		 if((strcmp(RootPtr->Key, K))== 0){
			 return RootPtr;
		 }else if((strcmp(K, RootPtr->Key)) < 0){
			 return BSTSearch(RootPtr->RPtr, K);
		 }else if((strcmp(K, RootPtr->Key)) > 0){
			 return BSTSearch(RootPtr->LPtr, K);
		 }else{
			 return NULL;
		 }
	 }else{
		 return NULL;
	 }

}


int BSTInsert (tBSTNodePtr* RootPtr, Dynamic_string* K, void* Content, tNodeType ntype)	{
//Vloží do stromu RootPtr uzel.
	 if(*RootPtr == NULL){
		 tBSTNodePtr x = malloc(sizeof(struct tBSTNode));
		 if(x == NULL)
			return 99;
		 x->Key = malloc(K->length);
		 if(x->Key == NULL)
			return 99;
		 strcpy(x->Key, K->string);
		 x->content = Content;
		 x->nodeType = ntype;
		 x->LPtr = NULL;
		 x->RPtr = NULL;
		 *RootPtr = x;
	 }else{
		 if((strcmp((*RootPtr)->Key, K->string))== 0){
			 (*RootPtr)->content = Content;
		 }else if((strcmp(K->string, (*RootPtr)->Key)) > 0){
			 BSTInsert(&((*RootPtr)->LPtr), K, Content, ntype);
		 }else if((strcmp(K->string, (*RootPtr)->Key)) < 0){
			 BSTInsert(&((*RootPtr)->RPtr), K, Content, ntype);
		 }
	 }
	return 0;
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
//Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
	 if(*RootPtr != NULL){
		 if((*RootPtr)->RPtr == NULL){
			 PtrReplaced->Key = (*RootPtr)->Key;
			 PtrReplaced->content = (*RootPtr)->content;
			 tBSTNodePtr x = *RootPtr;
			 *RootPtr = (*RootPtr)->LPtr;
			 free(x);
		 }else{
			 ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
		 }
	 }
}

void BSTDelete (tBSTNodePtr *RootPtr, char* K) 		{
//Zruší uzel stromu, který obsahuje klíč K.
	 if(*RootPtr != NULL){
		 if((*RootPtr)->Key > K){
			 BSTDelete(&(*RootPtr)->LPtr, K);
		 }else if((*RootPtr)->Key < K){
			 BSTDelete(&(*RootPtr)->RPtr, K);
		 }else{
			 tBSTNodePtr x = *RootPtr;
			 if(x->RPtr == NULL){
				 *RootPtr = x->LPtr;
				 free(x);
			 }else if(x->LPtr == NULL){
				 *RootPtr = x->RPtr;
				 free(x);
			 }else{
				 ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
			 }
		 }
	 }
}

void BSTDispose (tBSTNodePtr *RootPtr) {
//Zruší celý strom
	 if(*RootPtr != NULL){
		 BSTDispose(&(*RootPtr)->LPtr);
		 BSTDispose(&(*RootPtr)->RPtr);
		 free(*RootPtr);
		 *RootPtr = NULL;
	 }
}

//------------------------------------------

void symtableInit(tSymtable* tab){
	BSTInit(&(tab->root));
}

int symtableInsertF(tSymtable* tab, Dynamic_string* key){
	tInsideFunction *data = malloc(sizeof(tInsideFunction));
	data->returning = 0;
	data->declared = false;
	data->defined = false;
	data->local = NULL;
	data->parameters = 0;
	return BSTInsert(&(tab->root), key, data, nFunction);
}

int symtableInsertV(tSymtable* tab, Dynamic_string* key){
	tInsideVariable *data = malloc(sizeof(tInsideVariable));
	data->dataType = 0;
	return BSTInsert(&(tab->root), key, data, nVariable);
}

tBSTNodePtr symtableSearch(tSymtable* tab, char* key){
	return BSTSearch(tab->root, key);
}

void symtableDelete(tSymtable* tab, char* key){
	BSTDelete(&(tab->root), key);
}

void symtableDispose(tSymtable* tab){
	BSTDispose(&(tab->root));
}
