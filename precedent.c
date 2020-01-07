/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  precedent.c                                      *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "symstack.h"
#include "symtable.h"
#include "precedent.h"

Token* tokeng;
tSymtable* global;
tSymtable* local;
bool in_function2 = false;

void setGlobalVariables(Token* tok, tSymtable* glob){
	tokeng = tok;
	global = glob;
}
void setLocTable(tSymtable* loc){
	local = loc;
}

void setInFunction(bool in){
	in_function2 = in;
}

int precedent_analys(Token* help){

	symStack *stack = malloc(sizeof(symStack)); 	
	symstackInit(stack); 	
	symstackPush(stack, TOKEN_EOL);	
	int a;							// pushnuti prvni hodnoty EOL

	if(help != NULL){

		if((a = idkfunkce(stack, help))){
			return a;
		}
	}
	else
	{

	}
	
	if((a = idkfunkce(stack, tokeng))){

		return a;	

	}
	

	while(!(StackTopTerm(stack) == TOKEN_EOL && (tokeng->type == TOKEN_EOL || tokeng->type == TOKEN_COLON)))							// cyklus pobezi dokud nenarazi na znak konce radku
	{	

		get_token(tokeng);					// ziskame token

		a = idkfunkce(stack, tokeng);				// zavolame funkci pro zpracovani token
		if (a != SYNTAX_OK)
			return SEM_ERROR_OTHER;
	}

	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token* token){
		Token_type top = StackTopTerm(stack);						// do top nahrajeme nejvyssi terminal ze stacku



		
		
 

		if (token->type == TOKEN_ID || token->type == TOKEN_INTEGER || token->type == TOKEN_STRING  || token->type == TOKEN_FLOAT)							//TODO je potreba zjitit co vse jsou identifikatory
		{ 

			if (top == TOKEN_R_BRACKET || top == TOKEN_ID || top == TOKEN_FLOAT || top == TOKEN_INTEGER || top == TOKEN_STRING)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{	
				if (token->type == TOKEN_INTEGER)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_INTEGER);				// a znak identifikatoru
					stack->top->inte = token->attribute.integer;
				}
				else if (token->type == TOKEN_STRING)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_STRING);				// a znak identifikatoru
					stack->top->string = token->attribute.string;			
				}
				else if (token->type == TOKEN_FLOAT)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_FLOAT);				// a znak identifikatoru
					stack->top->flt = token->attribute.flt;
				}
				else if (token->type == TOKEN_ID)
				{
					tBSTNodePtr promnena;
					int g = in_function2;
					if(g == 1)
					{
						promnena = symtableSearch(local, token->attribute.string->string);

						if(promnena == NULL)
						{
							promnena = symtableSearch(global, token->attribute.string->string);	

							if(promnena == NULL)
								return SEM_ERROR_DEF;
						}
						
					}
					else
					{
						promnena = symtableSearch(global, token->attribute.string->string);	

						if(promnena == NULL)
							return SEM_ERROR_DEF;
					}
					

					tInsideVariable* var_content;
					var_content = promnena->content;
					if(var_content->dataType == 0){
						if(var_content->integer != 0){
							var_content->dataType = 1;
						}else if(var_content->string->length != 0){
							var_content->dataType = 3;
						}else if(var_content->flt != 0){
							var_content->dataType = 2;
						}
					}

					int typ = var_content->dataType;


					if (typ == 0) //pokud neni zatim nic
					{
						return SYNTAX_ERROR;
					}
					else if (typ == 1)	//pokud je integer
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_INTEGER);				// a znak identifikatoru
						stack->top->inte = var_content->integer;
						
					}
					else if (typ == 2)	//pokud je float
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_FLOAT);				// a znak identifikatoru
						stack->top->flt = var_content->flt;
						
					}
					else if (typ == 3)	// pokud je string
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_STRING);				// a znak identifikatoru
						stack->top->string = var_content->string;
						
					}
					else
					{
						return SYNTAX_ERROR;
					}
					char* lastidcko = malloc(token->attribute.string->length);
					strcpy(lastidcko, token->attribute.string->string);
					stack->top->nazev = lastidcko;
				}
			}
		}
		else if (token->type == TOKEN_L_BRACKET)		// jako token dojde (
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_PREC_ID)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				stackPushOpen(stack);				// jinak posle zacatek rozvoje
				symstackPush(stack,TOKEN_L_BRACKET);					// a znak identifikatoru
			}
		}
		else if (token->type == TOKEN_R_BRACKET)		// jako token dojde )
		{
			if (top == TOKEN_EOL || top == TOKEN_COLON )							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else if (top == TOKEN_L_BRACKET )						//pokud na na vrchu (
			{
				symstackPush(stack,TOKEN_R_BRACKET);					// pushni )
				symstackPush(stack,TOKEN_PREC_CLOSE);				// pushni konec rozvoje >
				reduction(stack);					// a proved redukci
			}
			else									//pokud je cokoliv jineho
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (token->type == TOKEN_EOL || token->type == TOKEN_COLON)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
		{
			if (top == TOKEN_EOL )							//pokud je na vrcholu eol tak jsme uspesne zredukovali celej vyraz
			{
				if (stack->top->tokenType == TOKEN_PREC_INTEGER)
				{
					token->attribute.keyword = RET_INT;

				}
				else if (stack->top->tokenType == TOKEN_PREC_FLOAT)
				{
					token->attribute.keyword = RET_FLOAT;
				}
				else if (stack->top->tokenType == TOKEN_PREC_STRING)
				{
					token->attribute.keyword = RET_STRING;
				}
				return SYNTAX_OK;
			}
			else if (top == TOKEN_L_BRACKET)						//pokud je na vrcholu ( tak nastane chyba
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else									// rekurzivne tocime dokud nenastane stav $E$ nebo nenastane chyba
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (token->type == TOKEN_PLUS || token->type == TOKEN_MINUS)					// jako token dojde +-
		{
			if (top == TOKEN_PLUS || top == TOKEN_MINUS || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV || top == TOKEN_INTEGER || top == TOKEN_STRING || top == TOKEN_FLOAT || top == TOKEN_INTEGER || top == TOKEN_STRING || top == TOKEN_FLOAT || top == TOKEN_R_BRACKET) //pokud je top + - * / // i )
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);				// jinak posle zacatek rozvoje
				if (token->type == TOKEN_PLUS)
				{
					symstackPush(stack,TOKEN_PLUS);					// a znak identifikatoru
				}
				else
				{
					symstackPush(stack,TOKEN_MINUS);					// a znak identifikatoru
				}
								
			}
		}
		else if (token->type == TOKEN_MUL || token->type == TOKEN_FLOAT_DIV || token->type == TOKEN_INT_DIV)					// jako token dojde * / //
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_INTEGER || top == TOKEN_STRING || top == TOKEN_FLOAT || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV)
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);
				if (token->type == TOKEN_MUL)
				{
					symstackPush(stack,TOKEN_MUL);
				}
				else if (token->type == TOKEN_FLOAT_DIV)
				{
					symstackPush(stack,TOKEN_FLOAT_DIV);
				}
				else
				{
					symstackPush(stack,TOKEN_INT_DIV);
				}
			}
		}
		else if (token->type == TOKEN_LESS || token->type == TOKEN_LEQ || token->type == TOKEN_MORE || token->type == TOKEN_MEQ || token->type == TOKEN_EQ || token->type == TOKEN_NEQ || token->type == TOKEN_ASSIGN)					// jako token dojde < <= > >= == !=	=
		{
			if (top >= TOKEN_MEQ && top <= TOKEN_NEQ)
			{
				return SEM_ERROR_OTHER; //nekompatibilni terminaly
			}
			else if (top == TOKEN_L_BRACKET || top == TOKEN_EOL)
			{
				stackPushOpen(stack);
				if (token->type == TOKEN_LESS)
				{
					symstackPush(stack,TOKEN_LESS);
				}
				else if (token->type == TOKEN_LEQ)
				{
					symstackPush(stack,TOKEN_LEQ);
				}
				else if (token->type == TOKEN_MORE)
				{
					symstackPush(stack,TOKEN_MORE);
				}
				else if (token->type == TOKEN_MEQ)
				{
					symstackPush(stack,TOKEN_MEQ);
				}
				else if (token->type == TOKEN_EQ)
				{
					symstackPush(stack,TOKEN_EQ);
				}
				else if (token->type == TOKEN_NEQ)
				{
					symstackPush(stack,TOKEN_NEQ);
				}
				else if (token->type == TOKEN_ASSIGN)
				{
					symstackPush(stack,TOKEN_ASSIGN);
				}
			}
			else
			{

				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >

				reduction(stack);					// proved redukci

				int b = idkfunkce(stack, token);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci

				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}	
			}
		}

		return SYNTAX_OK;	
}

Token_type StackTopTerm (symStack *stack){

	symStack *temp = malloc(sizeof(symStack)); 
	temp->top = stack->top;
	Token_type tokenhelp;
	if (temp->top == NULL)
	{
		return TOKEN_ERROR;
	}
	else
	{
		tokenhelp = stack->top->tokenType;
		while(tokenhelp > TOKEN_NEQ)
		{
	    	symStackItem* out = temp->top;
	    	if (out->next != NULL)
	    	{
	    	  	temp->top = out->next;
	    		tokenhelp = symstackTop(temp);
	    	}
	    	else
	    	{
	    		return TOKEN_ERROR;
	    	}
		}

		if (tokenhelp <= TOKEN_NEQ)
		{
			return tokenhelp;
		}
		else
		{
			return TOKEN_ERROR;
		}
	}
}

int stackPushOpen(symStack *stack){
	if(StackTopTerm(stack) == symstackTop(stack))
	{

		symstackPush(stack,TOKEN_PREC_OPEN);
		return 0;

	}else
	{

		symStackItem* temp = NULL;
		symStackItem* prev = NULL;
		symStackItem* nitem = (symStackItem*)malloc(sizeof(symStackItem));
		Token_type topterm=StackTopTerm(stack);
		Token_type help=symstackTop(stack);

		temp = stack->top;
		while(topterm != help)
		{

			
			help = temp->tokenType;

			if (topterm == help)
			{
				nitem->tokenType = TOKEN_PREC_OPEN;
  				nitem->next = temp;
    			prev->next = nitem;
    			return 0;

			}else
			{

				prev = temp;
				temp = prev->next;

			}
		}
	}
	return 0;
}


symStackItem* stackPosition(symStack *stack, int j){

	symStackItem* temp = stack->top;

	for (int i = 0; i < j; ++i)
	{
		temp = temp->next;
	}

	return temp;
}

int reduction(symStack *stack){
	symStackItem* temp = stack->top;
	Token_type topterm=StackTopTerm(stack);
	Token_type help=symstackTop(stack);
	int i=1;


	while(temp->tokenType != 26 || i >10) // zjisteni kolik zaznamu se nachazi v redukci pr < id + id > => 5
	{
		i++;
		temp = temp->next;
	}



	if(topterm == TOKEN_INTEGER || topterm == TOKEN_FLOAT || topterm == TOKEN_STRING) // nutno dodelat nevim jak na to potreba roylisit lokalni a globalni promenou
	{

		if (i == 3)
		{
			symstackPop(stack);
			help=symstackTop(stack);
			symStackItem* temp2 = malloc(sizeof(symStackItem));
			if(stack->top->nazev != NULL){
				if(in_function2){
					if(symtableSearch(local, stack->top->nazev) != NULL)
						printf("PUSHS LF@%s\n", stack->top->nazev);
					else if(symtableSearch(global, stack->top->nazev) != NULL)
						printf("PUSHS GF@%s\n", stack->top->nazev);
					else
						return SEM_ERROR_DEF;
				}else{
					printf("PUSHS GF@%s\n", stack->top->nazev);
				}
			}
			if (help == TOKEN_INTEGER)
			{
				if(stack->top->nazev == NULL)
					printf("PUSHS int@%d\n", stack->top->inte);
				temp2->inte = stack->top->inte;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_INTEGER);
				stack->top->inte = temp2->inte;
				
			}
			else if (help == TOKEN_FLOAT)
			{
				if(stack->top->nazev == NULL)
					printf("PUSHS float@%a\n", stack->top->flt);
				stack->top->flt = temp2->flt;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_FLOAT);
				stack->top->flt = temp2->flt;
			}
			else if (help ==TOKEN_STRING)
			{
				if(stack->top->nazev == NULL){
					printf("PUSHS string@");
					for (int i = 0; i < stack->top->string->length; i++){
						char c = stack->top->string->string[i];
						int ascii = (int)c;
						if(ascii <= 32 || ascii == 35 || ascii == 92){
							if(ascii < 10){
								printf("\\00%d", ascii);
							}else{
								printf("\\0%d", ascii);
							}
						}else{
							printf("%c", c);		
						}
					}
					printf("\n");
				}
				temp2->string = stack->top->string;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_STRING);
				stack->top->string = temp2->string;
			}
			else
			{
				free(temp2);
				return SYNTAX_ERROR;	
			}
			free(temp2);
			
		}
		else
		{

			return SYNTAX_ERROR;
		}
	}
	else if (topterm == TOKEN_R_BRACKET) // hotovo provede  redukci zavorek
	{

		if (i==5)
		{
			symStackItem* prvni = stackPosition(stack, 0);
			symStackItem* druhej = stackPosition(stack, 1);
			symStackItem* treti = stackPosition(stack, 2);
			symStackItem* ctvrtej = stackPosition(stack, 3);
			symStackItem* patej = stackPosition(stack, 4);
			symStackItem* temp2 = malloc(sizeof(symStackItem));;

			if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && druhej->tokenType == TOKEN_R_BRACKET && ctvrtej->tokenType == TOKEN_L_BRACKET)
			{
				temp2->tokenType = treti->tokenType;
				temp2->inte = treti->inte;
				temp2->flt = treti->flt;
				temp2->string = treti->string;
				symstackPopMore(stack, 5);
				symstackPush(stack, temp2->tokenType);
				stack->top->inte = temp2->inte;
				stack->top->flt = temp2->flt;
				stack->top->string = temp2->string;
				free(temp2);
				return SYNTAX_OK;
			}
			else
			{
				free(temp2);
				return SYNTAX_ERROR;
			}
		}
		else
		{
			return SYNTAX_ERROR;
		}
	}
	else if (topterm == TOKEN_L_BRACKET || topterm == TOKEN_EOL) //hotovo vyhodi chybu protoze to je zakazany stav
	{

		return SYNTAX_ERROR;
	}
	else if (topterm >= TOKEN_PLUS && topterm <= TOKEN_NEQ)
	{
		if (topterm == TOKEN_PLUS)
		{
			if (i == 5)
			{	
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);


				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_PLUS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType ==TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{

						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 420.0;
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType ==TOKEN_PREC_STRING)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("CONCAT GF@?CX? GF@?BX? GF@?AX?\n");
						printf("PUSHS GF@?CX?\n");
					}
					else
					{
						return SYNTAX_ERROR;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}

			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MINUS)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MINUS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("SUBS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType ==TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("SUBS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("SUBS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("SUBS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}

			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_FLOAT_DIV)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_FLOAT_DIV)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && druhej->inte == 0 )
					{
						return ZERO_DIVISION;
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && druhej->flt == 0)
					{
						return ZERO_DIVISION;
					}

					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("DIVS\n");



					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("DIVS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("DIVS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{

						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("DIVS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;	
					}
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_INT_DIV)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && druhej->tokenType == TOKEN_PREC_INTEGER && treti->tokenType == TOKEN_INT_DIV && ctvrtej->tokenType == TOKEN_PREC_INTEGER && patej->tokenType == TOKEN_PREC_OPEN)
				{
					if (druhej->inte != 0)
					{
					symstackPopMore(stack, 5);
					symstackPush(stack, TOKEN_PREC_INTEGER);
					stack->top->inte = 420;
					printf("IDIVS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}
					
				}
				else
				{
					return SEM_ERROR_TYPE;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MUL)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MUL)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("MULS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("MULS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("MULS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("MULS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}

				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MEQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MEQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
						
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType== TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}

				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MORE)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MORE)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("GTS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("GTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_LEQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_LEQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_LESS)
		{
			
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_LESS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("LTS\n");	
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("LTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else 
					{
						
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					
					return SYNTAX_ERROR;
				}
			}
			else
			{
				
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_EQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_EQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("EQS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("EQS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_ASSIGN)
		{
				return SYNTAX_ERROR;			
		}
		else if (topterm == TOKEN_NEQ)	//done
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_NEQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("INT2FLOATS\n");
						printf("EQS\n");
						printf("NOTS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else
		{
			return SYNTAX_ERROR;
		}
	}
	else
	{
		return SYNTAX_ERROR;
	}
	return 0;
}

    

