#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "symstack.h"
#include "precedent.h"
/*
		TODO funkce na ziskani top terminalu
		TODO funkce na doplneni zacatku rozvoje za top terminal // rozvoj(tStack, int)
		TODO funkce redukce   // reduction(tStac)
		TODO ZEPTAT SE NA EOL



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
if (top == 9 || top == 24 || top == 12 || top == 13 || top == 14)


8 6*/

int precedent_analys(Token *help)
{
	symStack *stack = malloc(sizeof(symStack)); 		// vytvoreni stacku
	symstackInit(stack); 								// init stacku
	symstackPush(stack, TOKEN_EOL);							// pushnuti prvni hodnoty EOL

	while(symstackTop(stack) != TOKEN_EOL && help->type != TOKEN_EOL)							// cyklus pobezi dokud nenarazi na znak konce radku
	{
		int a = idkfunkce(stack, help);
		get_token(help);							// ziskame token
		if (a != SYNTAX_OK)
			return SEM_ERROR_OTHER;
	}
	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token *help)
{

		Token_type top = symstackTop(stack);						// do top nahrajeme nejvyssi terminal ze stacku

		if (help->type == TOKEN_ID || help->type == TOKEN_INTEGER || help->type == TOKEN_STRING  || help->type == TOKEN_FLOAT)							//TODO je potreba zjitit co vse jsou identifikatory
		{
			if (top == TOKEN_R_BRACKET || top == TOKEN_PREC_ID)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				symstackPush(stack,TOKEN_PREC_OPEN);				// jinak posle zacatek rozvoje
				symstackPush(stack,TOKEN_PREC_ID);				// a znak identifikatoru
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
				symstackPush(stack,TOKEN_PREC_OPEN);				// jinak posle zacatek rozvoje
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
				symstackPush(stack,TOKEN_PREC_OPEN);				// jinak posle zacatek rozvoje
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
				symstackPush(stack,TOKEN_PREC_OPEN);
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
				symstackPush(stack,TOKEN_PREC_OPEN);
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

int reduction(symStack *stack){
	return SYNTAX_ERROR;
}

    

