#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "symstack.h"
#include "symtable.h"
#include "precedent.h"
/*
		TODO funkce na ziskani top terminalu done
		TODO funkce na doplneni zacatku rozvoje za top terminal // rozvoj(tStack, int) done
		TODO funkce redukce   // reduction(tStac) done
		TODO ZEPTAT SE NA EOL done
		TODO upravit volani funkce idk funkce
		todo vymyslet jak bude idk funkce vracet typ
		



    TOKEN_KEYWORD,				0
    TOKEN_ID,					1
    TOKEN_INTEGER,				2
    TOKEN_FLOAT,				3			
    TOKEN_STRING,				4
    TOKEN_EOF,					5
    TOKEN_EOL, 					6
    TOKEN_COMMA, // ,				7
    TOKEN_L_BRACKET, // (			8
    TOKEN_R_BRACKET, // )			9
    TOKEN_PLUS, // +				10
    TOKEN_MINUS, // -				11
    TOKEN_MUL, // *				12
    TOKEN_FLOAT_DIV, // /			13
    TOKEN_INT_DIV, // //			14
    TOKEN_MEQ, // >=				15
    TOKEN_MORE, // >				16
    TOKEN_LEQ, // <=				17
    TOKEN_LESS, //<				18
    TOKEN_EQ, // ==				19
    TOKEN_ASSIGN, // =				20
    TOKEN_NEQ, // !=				21
    TOKEN_PREC_OPEN, <				22
    TOKEN_PREC_CLOSE, >				23
    TOKEN_PREC_ID				24
    TOKEN_PREC_FLOAT
    TOKEN_PREC_INTEGER
    TOKEN_PREC_STRING
    TOKEN_ERROR
if (top == 9 || top == 24 || top == 12 || top == 13 || top == 14)


8 6*/
Token* token;
tSymtable* global;
tSymtable* local;
bool in_function2 = false;

void setGlobalVariables(Token* tok, tSymtable* glob){
	token = tok;
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
	
	if(help != NULL)
		if((a = idkfunkce(stack, help)))
			return a;
	if((a = idkfunkce(stack, token)))
		return a;	
	while(StackTopTerm(stack) != TOKEN_EOL && token->type != TOKEN_EOL)							// cyklus pobezi dokud nenarazi na znak konce radku
	{

		get_token(token);							// ziskame token
		a = idkfunkce(stack, token);				// zavolame funkci pro zpracovani tokenu

		if (a != SYNTAX_OK)
			return SEM_ERROR_OTHER;
	}
	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token* token){
		Token_type top = StackTopTerm(stack);						// do top nahrajeme nejvyssi terminal ze stacku


		if (token->type == TOKEN_ID || token->type == TOKEN_INTEGER || token->type == TOKEN_STRING  || token->type == TOKEN_FLOAT)							//TODO je potreba zjitit co vse jsou identifikatory
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_ID || TOKEN_FLOAT || TOKEN_INTEGER || TOKEN_STRING)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
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
					if(in_function2){
						promnena = symtableSearch(local, token->attribute.string->string);
						if(promnena == NULL){
							promnena = symtableSearch(global, token->attribute.string->string);						
							if(promnena == NULL)
								return SYNTAX_ERROR;
						}
						
					}else{
						promnena = symtableSearch(global, token->attribute.string->string);						
						if(promnena == NULL)
							return SYNTAX_ERROR;
					}
					

					tInsideVariable* var_content;
					var_content = promnena->content;
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
			if (top == TOKEN_EOL )							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
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
		else if (token->type == TOKEN_EOL)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
		{
			if (top == TOKEN_EOL )							//pokud je na vrcholu eol tak jsme uspesne zredukovali celej vyraz
			{
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
			if (top == TOKEN_PLUS || top == TOKEN_MINUS || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV || top == TOKEN_PREC_ID || top == TOKEN_R_BRACKET) //pokud je top + - * / // i )
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
			if (top == TOKEN_R_BRACKET || top == TOKEN_PREC_ID || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV)
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

	symStack *temp = = malloc(sizeof(symStack)); 
	temp->top = stack->top;
	Token_type tokenhelp;
	
	if (temp->top == NULL)
	{
		return TOKEN_ERROR;
	}
	else
	{
		tokenhelp = symstackTop(temp);

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
	    		return TOKEN_ERROR
	    	}
		}

		if (tokenhelp > TOKEN_NEQ)
		{
			return tokenhelp;
		}
		else
		{
			return TOKEN_ERROR;
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

	while(temp->tokenType != TOKEN_PREC_OPEN || i < 10) // zjisteni kolik zaznamu se nachazi v redukci pr < id + id > => 5
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
			symStackItem* temp2 = malloc(sizeof(symStackItem));;
			if (help == TOKEN_INTEGER)
			{
				temp2->inte = stack->top->inte;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_INTEGER);
				stack->top->inte = temp2->inte;
				
			}
			else if (help == TOKEN_FLOAT)
			{
				stack->top->flt = temp2->flt;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_FLOAT);
				stack->top->flt = temp2->flt;
			}
			else if (help ==TOKEN_STRING)
			{
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
			symStackItem* prvni = stackPosition(stack, 1);
			symStackItem* druhej = stackPosition(stack, 2);
			symStackItem* treti = stackPosition(stack, 3);
			symStackItem* ctvrtej = stackPosition(stack, 4);
			symStackItem* patej = stackPosition(stack, 5);
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
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);


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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType ==TOKEN_PREC_STRING) 			//TODO
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
						printf("POPS GF@$AXS\n");
						printf("POPS GF@$BXS\n");
						printf("CONCAT GF@$CXS GF@$BXS GF@$AXS\n");
						printf("PUSHS GF@$CXS\n");
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
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MINUS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("SUB\n");
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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
		else if (topterm == TOKEN_FLOAT_DIV)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_FLOAT_DIV)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && druhej->inte == 0 )
					{
						return SYNTAX_ERROR;
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && druhej->flt == 0)
					{
						return SYNTAX_ERROR;
					}

					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("POPS GF@$AX\n");
						printf("INT2FLOAT GF@$AXF GF@$AX\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
						printf("DIVS\n");
					}
					else
					{
						return SYNTAX_ERROR;	
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
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
		else if (topterm == TOKEN_MUL)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->flt = 42.0;
						printf("MULS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->flt = 42.0;
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
						printf("MULS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("MULS\n");
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
		else if (topterm == TOKEN_MEQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_MEQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						
					}
					else if (druhej->tokenType== TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
						printf("LTS\n");
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
		else if (topterm == TOKEN_MORE)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
		else if (topterm == TOKEN_LEQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TOKEN_LEQ)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 1;
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
		else if (topterm == TOKEN_LESS)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
		else if (topterm == TOKEN_EQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
		else if (topterm == TOKEN_ASSIGN)
		{
				return SYNTAX_ERROR;			
		}
		else if (topterm == TOKEN_NEQ)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 1);
				symStackItem* druhej = stackPosition(stack, 2);
				symStackItem* treti = stackPosition(stack, 3);
				symStackItem* ctvrtej = stackPosition(stack, 4);
				symStackItem* patej = stackPosition(stack, 5);

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
						printf("POPS GF@$AXF\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@$AXF\n");
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
	}
	else
	{
		return SYNTAX_ERROR;
	}
	return 0;
}

    

