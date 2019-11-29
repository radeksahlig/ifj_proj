#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
/*
		TODO funkce na ziskani top terminalu
		TODO funkce na doplneni zacatku rozvoje za top terminal // rozvoj(tStack, int)
		TODO funkce redukce   // reduction(tStac)
		TODO ZEPTAT SE NA EOL



	TOKEN_KEYWORD,					0
    TOKEN_ID,						1
    TOKEN_INTEGER,					2
    TOKEN_FLOAT,					3			
    TOKEN_STRING,					4
    TOKEN_EOF,						5
    TOKEN_EOL, 						 6
    TOKEN_COMMA, // ,				7
    TOKEN_L_BRACKET, // (			8
    TOKEN_R_BRACKET, // )			9
    TOKEN_PLUS, // +				10
    TOKEN_MINUS, // -				11
    TOKEN_MUL, // *					12
    TOKEN_FLOAT_DIV, // /			13
    TOKEN_INT_DIV, // //			14
    TOKEN_MEQ, // >=				15
    TOKEN_MORE, // >				16
    TOKEN_LEQ, // <=				17
    TOKEN_LESS, //<					18
    TOKEN_EQ, // ==					19
    TOKEN_ASSIGN, // =				20
    TOKEN_NEQ, // !=				21
    zacatek rozvoje <               22
    konec rozvoje >                 23
    identifikator 					24

if (top == 9 || top == 24 || top == 12 || top == 13 || top == 14)


8 6


*/
int precedent_analys()
{
	tStack *stack = malloc(sizeof(tStack)); 		// vytvoreni stacku
	stackInit(stack); 								// init stacku
	stackPush(stack, 6);							// pushnuti prvni hodnoty EOL
	Token help = NULL;								// inicializace pomocne promene

	while(gettopterm(stack, top) != 6 && help->type != TOKEN_EOL)							// cyklus pobezi dokud nenarazi na znak konce radku
	{
		get_token(help);							// ziskame token
		int a = idkfunkce(stack, help);
		if (a != 0)
		{
			return SEM_ERROR_OTHER;
			break;
		}
	}
	return 0;
}

int idkfunkce(tStack *stack, Token *help)
{

		int top;									// deklarace promene top
		gettopterm(stack, top);						// do top nahrajeme nejvyssi terminal ze stacku

		if (help->type == )							//TODO je potreba zjitit co vse jsou identifikatory
		{
			if (top == 9 || top == 24)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				stackPush(stack,22);				// jinak posle zacatek rozvoje
				stackPush(stack,24);				// a znak identifikatoru
			}
		}
		else if (help->type == TOKEN_L_BRACKET)		// jako token dojde (
		{
			if (top == 9 || top == 24)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				stackPush(stack,22);				// jinak posle zacatek rozvoje
				stackPush(stack,8);					// a znak identifikatoru
			}
		}
		else if (help->type == TOKEN_R_BRACKET)		// jako token dojde )
		{
			if (top == 6 )							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else if (top == 8 )						//pokud na na vrchu (
			{
				stackPush(stack,9);					// pushni )
				stackPush(stack,23);				// pushni konec rozvoje >
				reduction(stack);					// a proved redukci
			}
			else									//pokud je cokoliv jineho
			{
				stackPush(stack,23);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != 0)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (help->type == TOKEN_EOL)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
		{
			if (top == 6 )							//pokud je na vrcholu eol tak jsme uspesne zredukovali celej vyraz
			{
				return 0;
			}
			else if (top == 8)						//pokud je na vrcholu ( tak nastane chyba
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else									// rekurzivne tocime dokud nenastane stav $E$ nebo nenastane chyba
			{
				stackPush(stack,23);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != 0)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (help->type == TOKEN_PLUS || help->type == TOKEN_MINUS)					// jako token dojde +-
		{
			if (top == 10 || top == 11 || top == 12 || top == 13 || top == 14 || top == 24 || top == 9) //pokud je top + - * / // i )
			{
				stackPush(stack,23);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != 0)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPush(stack,22);				// jinak posle zacatek rozvoje
				if (help->type == TOKEN_PLUS)
				{
					stackPush(stack,10);					// a znak identifikatoru
				}
				else
				{
					stackPush(stack,11);					// a znak identifikatoru
				}
								
			}
		}
		else if (help->type == TOKEN_MUL || help->type == TOKEN_FLOAT_DIV || help->type == TOKEN_INT_DIV)					// jako token dojde * / //
		{
			if (top == 9 || top == 24 || top == 12 || top == 13 || top == 14)
			{
				stackPush(stack,23);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != 0)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPush(stack,22);
				if (help->type == TOKEN_MUL)
				{
					stackPush(stack,12);
				}
				else if (help->type == TOKEN_FLOAT_DIV)
				{
					stackPush(stack,13);
				}
				else
				{
					stackPush(stack,14);
				}
			}
		}
		else if (help->type == TOKEN_LESS || help->type == TOKEN_LEQ || help->type == TOKEN_MORE || help->type == TOKEN_MEQ || help->type == TOKEN_EQ || help->type == TOKEN_NEQ || help->type == TOKEN_ASSIGN)					// jako token dojde < <= > >= == !=	=
		{
			if (top >= 15 && top <= 21)
			{
				return SEM_ERROR_OTHER; //nekompatibilni terminaly
			}
			else if (top == 8 || top == 6)
			{
				stackPush(stack,22);
				if (help->type == TOKEN_LESS)
				{
					stackPush(stack,18);
				}
				else if (help->type == TOKEN_LEQ)
				{
					stackPush(stack,17);
				}
				else if (help->type == TOKEN_MORE)
				{
					stackPush(stack,16);
				}
				else if (help->type == TOKEN_MEQ)
				{
					stackPush(stack,15);
				}
				else if (help->type == TOKEN_EQ)
				{
					stackPush(stack,19);
				}
				else if (help->type == TOKEN_NEQ)
				{
					stackPush(stack,21);
				}
				else if (help->type == TOKEN_ASSIGN)
				{
					stackPush(stack,20);
				}
			}
			else
			{
				stackPush(stack,23);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, help);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != 0)
				{
					return SEM_ERROR_OTHER;
				}	
			}
		}
		return 0;	

}

    

