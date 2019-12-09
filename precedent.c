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
if (top == 9 || top == 24 || top == 12 || top == 13 || top == 14)


8 6*/

int precedent_analys(tSymtable* table){
	token *help = NULL;
	symStack *stack = malloc(sizeof(symStack)); 		// vytvoreni stacku
	symstackInit(stack); 								// init stacku
	symstackPush(stack, TOKEN_EOL);							// pushnuti prvni hodnoty EOL

	get_token(help);							// ziskame token
	int a = idkfunkce(stack, help, table);				// zavolame funkci pro zpracovani tokenu

	while(StackTopTerm(stack) != TOKEN_EOL && help->type != TOKEN_EOL)							// cyklus pobezi dokud nenarazi na znak konce radku
	{

		get_token(help);							// ziskame token
		int a = idkfunkce(stack, help, table);				// zavolame funkci pro zpracovani tokenu

		if (a != SYNTAX_OK)
			return SEM_ERROR_OTHER;
	}
	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token *help, tSymtable* table){

		Token_type top = StackTopTerm(stack);						// do top nahrajeme nejvyssi terminal ze stacku

		if (help->type == TOKEN_ID || help->type == TOKEN_INTEGER || help->type == TOKEN_STRING  || help->type == TOKEN_FLOAT)							//TODO je potreba zjitit co vse jsou identifikatory
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_ID || TOKEN_FLOAT || TOKEN_INTEGER || TOKEN_STRING)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				if (help->type == TOKEN_INTEGER)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_INTEGER);				// a znak identifikatoru
				}
				else if (help->type == TOKEN_STRING)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_STRING);				// a znak identifikatoru
				}
				else if (help->type == TOKEN_FLOAT)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TOKEN_FLOAT);				// a znak identifikatoru
				}
				else if (help->type == TOKEN_ID)
				{
					tBSTNodePtr promnena = symtableSearch(table, help.attribute.string->string);
					if (promnena == NULL)
					{
						return SYNTAX_ERROR;
					}
					tInsideVariable* var_content;
					var_content = promnena->content;
					int typ = var_content->dataType;

					if (typ == 0) //pokud neni zatim nic
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_ID);				// a znak identifikatoru
					}
					else if (typ == 1)	//pokud je integer
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_INTEGER);				// a znak identifikatoru
					}
					else if (typ == 2)	//pokud je float
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_FLOAT);				// a znak identifikatoru
					}
					else if (typ == 3)	// pokud je string
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TOKEN_STRING);				// a znak identifikatoru
					}
					else
					{
						return SYNTAX_ERROR;
					}
				}
			}
		}
		else if (help->type == TOKEN_L_BRACKET)		// jako token dojde (
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
		else if (help->type == TOKEN_R_BRACKET)		// jako token dojde )
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
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (help->type == TOKEN_EOL)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
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
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (help->type == TOKEN_PLUS || help->type == TOKEN_MINUS)					// jako token dojde +-
		{
			if (top == TOKEN_PLUS || top == TOKEN_MINUS || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV || top == TOKEN_PREC_ID || top == TOKEN_R_BRACKET) //pokud je top + - * / // i )
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);				// jinak posle zacatek rozvoje
				if (help->type == TOKEN_PLUS)
				{
					symstackPush(stack,TOKEN_PLUS);					// a znak identifikatoru
				}
				else
				{
					symstackPush(stack,TOKEN_MINUS);					// a znak identifikatoru
				}
								
			}
		}
		else if (help->type == TOKEN_MUL || help->type == TOKEN_FLOAT_DIV || help->type == TOKEN_INT_DIV)					// jako token dojde * / //
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_PREC_ID || top == TOKEN_MUL || top == TOKEN_FLOAT_DIV || top == TOKEN_INT_DIV)
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);
				if (help->type == TOKEN_MUL)
				{
					symstackPush(stack,TOKEN_MUL);
				}
				else if (help->type == TOKEN_FLOAT_DIV)
				{
					symstackPush(stack,TOKEN_FLOAT_DIV);
				}
				else
				{
					symstackPush(stack,TOKEN_INT_DIV);
				}
			}
		}
		else if (help->type == TOKEN_LESS || help->type == TOKEN_LEQ || help->type == TOKEN_MORE || help->type == TOKEN_MEQ || help->type == TOKEN_EQ || help->type == TOKEN_NEQ || help->type == TOKEN_ASSIGN)					// jako token dojde < <= > >= == !=	=
		{
			if (top >= TOKEN_MEQ && top <= TOKEN_NEQ)
			{
				return SEM_ERROR_OTHER; //nekompatibilni terminaly
			}
			else if (top == TOKEN_L_BRACKET || top == TOKEN_EOL)
			{
				stackPushOpen(stack);
				if (help->type == TOKEN_LESS)
				{
					symstackPush(stack,TOKEN_LESS);
				}
				else if (help->type == TOKEN_LEQ)
				{
					symstackPush(stack,TOKEN_LEQ);
				}
				else if (help->type == TOKEN_MORE)
				{
					symstackPush(stack,TOKEN_MORE);
				}
				else if (help->type == TOKEN_MEQ)
				{
					symstackPush(stack,TOKEN_MEQ);
				}
				else if (help->type == TOKEN_EQ)
				{
					symstackPush(stack,TOKEN_EQ);
				}
				else if (help->type == TOKEN_NEQ)
				{
					symstackPush(stack,TOKEN_NEQ);
				}
				else if (help->type == TOKEN_ASSIGN)
				{
					symstackPush(stack,TOKEN_ASSIGN);
				}
			}
			else
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}	
			}
		}
		return SYNTAX_OK;	
}

Token_type StackTopTerm (symStack *stack){

	symStack *temp = stack;
	
	Token_type tokenhelp;
	tokenhelp = symstackTop(temp);

	while(tokenhelp <= TOKEN_NEQ){

	symStackItem* out = temp->top;
    temp->top = out->next;
    tokenhelp = symstackTop(temp);
	}

	if (tokenhelp > TOKEN_NEQ)
	{
		return tokenhelp

	}else{

		return SYNTAX_ERROR;
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
}


Token_type stackPosition(symStack *stack, int j){

	symStackItem* temp = stack->top;

	for (int i = 0; i < j; ++i)
	{
		temp = temp->next;
	}

	return temp->tokenType;
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


	if(topterm == TOKEN_ID || topterm == TOKEN_INTEGER || topterm == TOKEN_FLOAT || topterm == TOKEN_STRING) // hotovo redukce id  todo rozdelit id podle typu v idk funkci
	{
		if (i == 3)
		{
			symstackPop(stack);
			help=symstackTop(stack);
			if (help == TOKEN_ID)
			{
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_ID);
			}
			else if (help == TOKEN_INTEGER)
			{
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_INTEGER);
			}
			else if (help == TOKEN_FLOAT)
			{
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_FLOAT);
			}
			else if (TOKEN_STRING)
			{
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_STRING);
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
	else if (topterm == TOKEN_R_BRACKET) // hotovo provede  redukci zavorek
	{

		if (i==5)
		{
			Token_type prvni = stackPosition(stack, 1);
			Token_type druhej = stackPosition(stack, 2);
			Token_type treti = stackPosition(stack, 3);
			Token_type ctvtej = stackPosition(stack, 4);
			Token_type patej = stackPosition(stack, 5);

			if (prvni == TOKEN_PREC_CLOSE && patej = TOKEN_PREC_OPEN && druhej == TOKEN_R_BRACKET && ctvtej == TOKEN_L_BRACKET)
			{
				symstackPopMore(stack, 5);
				symstackPush(stack, treti);
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
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej = TOKEN_PREC_OPEN && treti == TOKEN_PLUS)
				{
					if (druhej == TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
					else if ((druhej == TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej == TOKEN_PREC_INTEGER && ctvtej ==TOKEN_PREC_FLOAT))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}
					else if (druhej == TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}
					else if (druhej == TOKEN_PREC_STRING && ctvtej ==TOKEN_PREC_STRING)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
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
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej = TOKEN_PREC_OPEN && treti == TOKEN_MINUS)
				{
					if (druhej == TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
					else if ((druhej == TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej == TOKEN_PREC_INTEGER && ctvtej ==TOKEN_PREC_FLOAT))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}
					else if (druhej == TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}
					else if (druhej == TOKEN_PREC_STRING && ctvtej ==TOKEN_PREC_STRING)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
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
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_FLOAT_DIV && ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER)))
				{
					symstackPopMore(stack, 5);
					symstackPush(stack, TOKEN_PREC_FLOAT);
				}

			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_INT_DIV)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && druhej == TOKEN_PREC_INTEGER && treti == TOKEN_INT_DIV && ctvtej == TOKEN_PREC_INTEGER && patej == TOKEN_PREC_OPEN)
				{
					symstackPopMore(stack, 5);
					symstackPush(stack, TOKEN_PREC_INTEGER);
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MUL)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_MUL)
				{
					if (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
					else if (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}
					else if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
					}

				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MEQ)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_MEQ)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_MORE)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_MORE)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_LEQ)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_LEQ)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_LESS)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_LESS)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_EQ)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_EQ)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_ASSIGN)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_ASSIGN)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else if (topterm == TOKEN_NEQ)
		{
			if (i == 5)
			{
				Token_type prvni = stackPosition(stack, 1);
				Token_type druhej = stackPosition(stack, 2);
				Token_type treti = stackPosition(stack, 3);
				Token_type ctvtej = stackPosition(stack, 4);
				Token_type patej = stackPosition(stack, 5);

				if (prvni == TOKEN_PREC_CLOSE && patej == TOKEN_PREC_OPEN && treti == TOKEN_NEQ)
				{
					if ((druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_INTEGER) || (druhej== TOKEN_PREC_FLOAT && ctvtej == TOKEN_PREC_FLOAT) || (druhej== TOKEN_PREC_INTEGER && ctvtej == TOKEN_PREC_INTEGER ))
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
					}
				}
				else
				{
					SYNTAX_ERROR;
				}
			}
			else
			{
				SYNTAX_ERROR;
			}
		}
		else
		{
			SYNTAX_ERROR;
		}
	}
	else
	{
		return SYNTAX_ERROR;
	}
	return 0;
}

    

