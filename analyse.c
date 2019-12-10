#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "scanner.h"
#include "precedent.h"
#include "analyse.h"
#include "symtable.h"
#include "string.h"


#define GET_TOKEN_CHECK_TYPE(_type) do{\
    GET_TOKEN();\
    if(token.type != (_type))\
        return SYNTAX_ERROR;\
}while(0)

#define GET_TOKEN_CHECK_KEYWORD(_keyword) do{\
    GET_TOKEN();\
    if(token.type != TOKEN_KEYWORD || token.attribute.keyword !=(_keyword))\
        return SYNTAX_ERROR;\
}while(0)

#define GET_TOKEN_CHECK_RULE(rule) do{\
    GET_TOKEN();\
    if((retval = rule()))\
        return retval;\
}while(0)

#define CHECK_PRECEDENCE(token) do{\
    if((retval = precedent_analys(&token)))\
        return retval;\
}while(0)

#define GET_TOKEN() do{\
	if((retval = get_token(&token)))\
		return retval;\
}while(0)

#define CHECK_STMT(this_indent, cur_indent) do{\
    if((retval = stmt((this_indent), (cur_indent))))\
        return retval;\
}while(0)

#define CHECK_NUM_INDENT(cur_indent, prev_indent)do{\
	GET_TOKEN();\
        while(token.type == TOKEN_INDENT){\
            (cur_indent)++;\
            GET_TOKEN();\
        }\
        if((cur_indent) == (prev_indent))\
            return SYNTAX_ERROR;\
}while(0)

#define CHECK_NUM_DEDENT(prev_indent, base_indent)do{\
        while(token.type == TOKEN_DEDENT){\
            (prev_indent)--;\
    	    if((prev_indent) == (base_indent)){\
		GET_TOKEN();\
		return SYNTAX_OK;\
	    }\
	    GET_TOKEN();\
        }\
        if((prev_indent) == (base_indent)){\
            return SYNTAX_OK;\
        }else\
            return SYNTAX_ERROR;\
}while(0)

#define INSERT_VEST_FUNCTION(s, l)do{\
	if((retval = insertVestFunction(s, l)))\
		return retval;\
}while(0)

Token token;
tSymtable* global;
tSymtable* local;
int not_defined = 0;
bool in_function = false;
int global_label = 1;


/*Token_type tokeny[] = {TOKEN_ID, TOKEN_L_BRACKET, TOKEN_STRING, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_ID, TOKEN_ASSIGN, TOKEN_ID, TOKEN_L_BRACKET, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_EOF};*/
//-----------------------------------------------------------------------------------
/*
print('Zadejte cislo pro vypocet faktorialu: ')
a = inputi()
if a < 0.0: # pred porovnanim probehne implicitni konverze int na float
print('Faktorial nelze spocitat')
else:
vysl = factorial(a)
print('Vysledek je:', vysl)
*/
/*
Token_type tokeny[] = {TOKEN_ID, TOKEN_L_BRACKET, TOKEN_STRING, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_ID, TOKEN_ASSIGN, TOKEN_ID, TOKEN_L_BRACKET, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON, TOKEN_EOL, TOKEN_INDENT, TOKEN_INDENT, TOKEN_ID, TOKEN_L_BRACKET, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_DEDENT, TOKEN_DEDENT, TOKEN_KEYWORD, TOKEN_COLON, TOKEN_EOL, TOKEN_INDENT, TOKEN_ID, TOKEN_ASSIGN, TOKEN_ID, TOKEN_L_BRACKET, TOKEN_ID, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_ID, TOKEN_L_BRACKET, TOKEN_STRING, TOKEN_COMMA, TOKEN_ID, TOKEN_R_BRACKET, TOKEN_EOL, TOKEN_DEDENT, TOKEN_EOF, TOKEN_EOF, TOKEN_EOF};

Keyword keywordy[] = {KEYWORD_IF, KEYWORD_ELSE};


//----------------------------------------------------------------------------------
int i = 0;
int j = 0;
int get_token(Token *token){
	if(i < 50){
		if(tokeny[i] == TOKEN_KEYWORD){
			token->type = TOKEN_KEYWORD;
			token->attribute.keyword = keywordy[j];
			j++;		
		}else{
			token->type = tokeny[i];
		}
	}
	i++;
	return 0;
}*/
int new_token(Token *token){
	int retval= get_token(token);
	switch(token->type){
	    case TOKEN_EMPTY_FILE : printf("token_empty \n");break;
	    case TOKEN_KEYWORD : printf("token_keyword \n");break;
	    case TOKEN_ID :printf("token_id \n");break;
	    case TOKEN_INTEGER : printf("token_int \n");break;
	    case TOKEN_FLOAT : printf("token_float \n");break;
	    case TOKEN_STRING : printf("token_string \n");break;
	    case TOKEN_EOF : printf("token_eof \n");break;
	    case TOKEN_EOL : printf("token_eol \n");break;
	    case TOKEN_COMMA : printf("token_comma \n");break;
	    case TOKEN_L_BRACKET : printf("token_( \n");break;
	    case TOKEN_R_BRACKET : printf("token_) \n");break;
	    case TOKEN_PLUS : printf("token_+ \n");break;
	    case TOKEN_MINUS : printf("token_- \n");break;
	    case TOKEN_MUL : printf("token_* \n");break;
	    case TOKEN_FLOAT_DIV : printf("token_/ \n");break;
	    case TOKEN_INT_DIV : printf("token_// \n");break;
	    case TOKEN_MEQ : printf("token_>= \n");break;
	    case TOKEN_MORE : printf("token_> \n");break;
	    case TOKEN_LEQ : printf("token_<= \n");break;
	    case TOKEN_LESS : printf("token_< \n");break;
	    case TOKEN_EQ : printf("token_== \n");break;
	    case TOKEN_COLON : printf("token_: \n");break;
	    case TOKEN_ASSIGN : printf("token_= \n");break;
	    case TOKEN_NEQ : printf("token_!= \n");break;
	    case TOKEN_INDENT : printf("token_indent --------------->\n");break;
	    case TOKEN_DEDENT : printf("token_dedent <---------------\n");break;
	    default : printf("nemůže nastat !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
	if(token->type == TOKEN_STRING){
		printf(" ## %s ##\n", token->attribute.string->string);	
	}
	if(retval != 0){
		printf("%d return new_token\n", retval);
		return retval;}
	return 0;
}

int prog(){
	//printf("---in prog\n");
	int retval;
    	int cur_indent = 0;
    	int prev_indent = 0;
	int label = global_label;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_DEF){
		//1. <prog> -> def id (<params>) : EOL <stmt> EOL <prog>
		//printf("---in def\n");

		GET_TOKEN_CHECK_TYPE(TOKEN_ID);

		in_function = true;
		bool was_declared = false;
		int param = 0;
		//printf("%s -- funkce \n",token.attribute.string->string);
		
		tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
		tInsideFunction* fce_content;
		
		char* fce = malloc(token.attribute.string->length);
		strcpy(fce, token.attribute.string->string);

		if(funkce != NULL){//Funkce je declarovaná
			if(funkce->nodeType == nVariable)
				return SEM_ERROR_DEF;
			else{
				fce_content = funkce->content;
				if(fce_content->defined == true)//Funkce je definovaná
					return SEM_ERROR_DEF;

				fce_content->defined = true;
				not_defined -= 1;
				was_declared = true;					
						
				param = fce_content->parameters;			
				fce_content->parameters = 0;

				local = malloc(sizeof(tSymtable));
				symtableInit(local);

				fce_content->local = local;
			}
		}else{//Funkce se vloží do symtable
			//printf("Vkládám funkci do global -> %s\n", token.attribute.string->string);
			if((retval = symtableInsertF(global, token.attribute.string)))
				return retval;
			tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
			if(funkce == NULL)
				return INTERNAL_ERROR;

			fce_content = funkce->content;	

			fce_content->declared = true;
			fce_content->defined = true;

			local = malloc(sizeof(tSymtable));
			symtableInit(local);

			fce_content->local = local;
		}
		

		GET_TOKEN_CHECK_TYPE(TOKEN_L_BRACKET);
	
		GET_TOKEN();
		if((retval = params(fce_content)))
			return retval;
		if(token.type != TOKEN_R_BRACKET)
			return SYNTAX_ERROR;
		
		//printf("Funkce %s má %d(%d) parametrů\n", token.attribute.string->string, fce_content->parameters, param);
		
		if((param != fce_content->parameters) && was_declared)
			return SEM_ERROR_PARAM;

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		

		//Výpis začátku funkce
		printf("JUMP !konec!%s\n", fce);
		printf("#Začátek funkce %s : \n", fce);
		printf("LABEL !%s\n", fce);
		printf("PUSHFRAME\n");
		printf("DEFVAR LF@%%retval\n");
		printf("MOVE LF@%%retval nil@nil\n");
		for (int i = 1; i <= fce_content->parameters; i++)
		{
			if(i == 2){
				printf("DEFVAR LF@%s\n", fce_content->prvniparam);
				printf("MOVE LF@%s LF@%%%d\n", fce_content->prvniparam, i);
			}else{
				printf("DEFVAR LF@%s\n", (char *)fce_content->paramName[i-1]);
				printf("MOVE LF@%s LF@%%%d\n", (char *) fce_content->paramName[i-1], i);
			}		
		}
		CHECK_NUM_INDENT(cur_indent, prev_indent);

		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		
		//Výpis konce funkce
		printf("POPFRAME\n");
		printf("RETURN\n");
		printf("#Konec funkce %s\n", fce);
		printf("LABEL !konec!%s\n", fce);
		free(fce);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		in_function = false;
		return prog();

	}else if(token.type == TOKEN_EOL){
		//printf("---in eol prog\n");
		//2. <prog> -> EOL <prog>
		GET_TOKEN();
		return prog();
	}else if(token.type == TOKEN_EOF){
		//printf("---in eof -- nedefinovaných fcí je : %d\n", not_defined);
		//3. <prog> -> EOF
		//Projít nezavolaný fce
		if(not_defined == 0)
			return SYNTAX_OK;
		return SEM_ERROR_DEF;
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		//printf("---in if prog\n");        
		//4. <prog> -> if <expr> : EOL <stmt> else : EOL <stmt> <prog>
		printf("#Začátek ifu v progu\n");

		GET_TOKEN();
		CHECK_PRECEDENCE(token);

		printf("DEFVAR $ifprom%d\n", label);
		printf("POPS $ifprom%d\n", label);

		//podmínka skosku

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);

		
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;

		//printf("---in else prog\n");
		if(token.type != TOKEN_KEYWORD || token.attribute.keyword != KEYWORD_ELSE)
			return SYNTAX_ERROR;
  
		printf("JUMP $end$ifu%d\n", label);
		printf("LABEL $else$%d\n", label);


		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;

		printf("LABEL $end$ifu%d\n", label);
		global_label++;
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		//printf("---in while prog\n");
		//5. <prog> -> while <expr> : EOL <stmt> EOL <prog>

		printf("#Začátek while v progu\n");
		printf("LABEL $while$start%d\n", label);

		GET_TOKEN();
		CHECK_PRECEDENCE(token);
		
		printf("DEFVAR $whileprom%d\n", label);
		printf("POPS $whileprom%d\n", label);
		//Podmíněný skok

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		
		printf("JUMP $while$start%d\n", label);
		printf("LABEL $while$end%d\n", label);

		global_label++;

		return prog();
	}else if(token.type == TOKEN_ID){
		//printf("---in id prog\n");
		//6. <prog> -> id<def> EOL <prog>
		if((retval = def()))
			return retval;
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;		
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){
		//printf("---in pass prog\n");        
		//7. <prog> -> pass EOL <prog>
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		return prog();
		}
	if(token.type == TOKEN_DEDENT){
	    return prog();
	}
	return SYNTAX_ERROR;
}

static int stmt(int prev_indent, int cur_indent){
	int retval;
	int this_indent = cur_indent;
	int label = global_label;
	//printf("---in stmt indent : %d - %d token\n", prev_indent, cur_indent);
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		//8. <stmt> ->  if <expr> : EOL <stmt> else : EOL <stmt> EOL <stmt>
		//printf("---in if stmt\n");
		printf("#začátek ifu v stmt\n");

		GET_TOKEN(); 
		CHECK_PRECEDENCE(token);

		printf("DEFVAR $ifprom%d\n", label);
		printf("POPS $ifprom%d\n", label);
		
		//podmínka skoku

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;

		//printf("---in else stmt\n");
		if(token.type != TOKEN_KEYWORD || token.attribute.keyword != KEYWORD_ELSE)
			return SYNTAX_ERROR;
  
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		
		printf("JUMP $end$ifu%d\n", label);
		printf("LABEL $else$%d\n", label);

		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;

		printf("LABEL $end$ifu%d\n", label);
		global_label++;

		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		printf("---in while stmt\n"); 
		//9. <stmt> ->  while <expr> : EOL <stmt> EOL <stmt>

		printf("#Začátek while v stmt\n");
		printf("LABEL $while$start%d\n", label);

		GET_TOKEN();
		CHECK_PRECEDENCE(token);

		printf("DEFVAR $whileprom%d\n", label);
		printf("POPS $whileprom%d\n", label);
		//Podmíněný skok

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;

		printf("JUMP $while$start%d\n", label);
		printf("LABEL $while$konec%d\n", label);

		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_ID){
		//printf("---in id stmt\n"); 
		//11. <stmt> -> id <def> EOL <stmt> 
		if((retval = def()))
			return retval;
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;
		GET_TOKEN();
        	return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){
		//printf("---in pass stmt\n");         
		//13. <stmt> -> pass EOL <stmt>
	        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
	        GET_TOKEN();
	        return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_EOL){
		//printf("---in eol stmt\n"); 
		//10. <stmt> -> EOL <stmt>
		GET_TOKEN();
		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_RETURN){
		//printf("---in return stmt\n"); 
		//12. <stmt> -> return <expr> EOL <stmt>
		GET_TOKEN();
		CHECK_PRECEDENCE(token);
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;
		printf("POPS LF@%%retval\n");
		
		CHECK_NUM_DEDENT(cur_indent, prev_indent);
	}
	if(token.type == TOKEN_DEDENT){
	//printf("---in dedent stmt %d - %d token\n", prev_indent, cur_indent);
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
    }
	//printf("---in stmt end\n"); 
    //14. <stmt> -> ε
	//není return -> retval = None
	return SYNTAX_OK;
}

static int params(tInsideFunction* funkce){
	int retval;
	//printf("---in params\n");
	if(token.type == TOKEN_ID){
        //16. <params> -> id <param_n>;

        tBSTNodePtr idcko = symtableSearch(global, token.attribute.string->string);
	if(idcko != NULL){
		if(idcko->nodeType == nFunction)
			return SEM_ERROR_DEF;
	}
	
	if(funkce->parameters > 10)
		return INTERNAL_ERROR;

	if((retval = symtableInsertV(local, token.attribute.string)))
		return retval;
	//printf("Vkládám proměnnou %s do local %p\n", token.attribute.string->string, (void *)local);
	idcko = symtableSearch(local, token.attribute.string->string);
	if(idcko == NULL)
		return INTERNAL_ERROR;
		
	initVariable(idcko->content);

	strcpy((char *) funkce->paramName[funkce->parameters],token.attribute.string->string);
	funkce->parameters += 1;
	//printf("Přidám parametr %d s nazvem %s\n", funkce->parameters, funkce->paramName[funkce->parameters-1]);
	GET_TOKEN();
        return param_n(funkce);
	}
    //15. <params> -> ε
	return SYNTAX_OK;
}

static int param_n(tInsideFunction* funkce){
	int retval;
	//printf("---in param_n\n");
	if(token.type == TOKEN_COMMA){
        //18. <param_n> -> , id <param_n>
	        GET_TOKEN_CHECK_TYPE(TOKEN_ID);
		tBSTNodePtr idcko = symtableSearch(global, token.attribute.string->string);
		if(idcko != NULL && idcko->nodeType == nFunction)
			return SEM_ERROR_DEF;

		if(funkce->parameters > 10)
			return INTERNAL_ERROR;

		if((retval = symtableInsertV(local, token.attribute.string)))
			return retval;
	
		//printf("Vkládám proměnnou %s do local %p\n", token.attribute.string->string, (void *)local);
		idcko = symtableSearch(local, token.attribute.string->string);
		if(idcko == NULL)
			return INTERNAL_ERROR;

		initVariable(idcko->content);

		strcpy((char *)funkce->paramName[funkce->parameters],token.attribute.string->string);
		if(funkce->parameters == 1)
			strcpy(funkce->prvniparam, token.attribute.string->string);
		funkce->parameters += 1;
		GET_TOKEN();
		return param_n(funkce);	
	}else if(token.type == TOKEN_R_BRACKET)
		return SYNTAX_OK;
    return SYNTAX_ERROR;
}

static int def(){
	int retval;
	//printf("---in def\n");

	Token pid = token;
	GET_TOKEN();

	if(token.type == TOKEN_L_BRACKET){
		//19. <def> -> (<arg>)
		//printf("Nazev fce - %s\n", pid.attribute.string->string);
			
		tBSTNodePtr funkce = symtableSearch(global, pid.attribute.string->string);
		tInsideFunction* fce_content;
		if(funkce == NULL){//Funkce není definovaná
			if((retval = symtableInsertF(global, pid.attribute.string)))
				return retval;
			tBSTNodePtr funkce = symtableSearch(global, pid.attribute.string->string);
			if(funkce == NULL)
				return INTERNAL_ERROR;

			fce_content = funkce->content;
			fce_content->declared = true;
			fce_content->defined = false;
			not_defined += 1;
		}else if(funkce->nodeType == nFunction){//Funkce je definovaná
			fce_content = funkce->content;
		}else{
			return SEM_ERROR_DEF;		
		}
		int param2 = fce_content->parameters;
		int param = fce_content->parameters;
		if((!fce_content->defined)){
			param = 0;
		}
		
		
		GET_TOKEN();
		//Výpis
		printf("#Tvorba framu pro %s\n", pid.attribute.string->string);
		printf("CREATEFRAME\n");

		char* fce = malloc(pid.attribute.string->length);
		strcpy(fce, pid.attribute.string->string);
		int count = 1;
		if((retval= arg(&param, fce_content->defined, &count)))
			return retval;

		//Kontrola počtu parametrů
		if(fce_content->defined){
			if(param != 0)
				return SEM_ERROR_PARAM;
		}else{
			if(param2 == param || param2 == 0)
				fce_content->parameters = param;
			else
				return SEM_ERROR_PARAM;
		}
		
		if(token.type != TOKEN_R_BRACKET)
			return SYNTAX_ERROR;

		printf("CALL !%s\n", fce);
		free(fce);
		GET_TOKEN();
		return SYNTAX_OK;
	}else if(token.type == TOKEN_ASSIGN){
		//20. <def> -> = <rovn>
		tBSTNodePtr idcko = symtableSearch(global, pid.attribute.string->string);
		tInsideVariable* var_content;
		if(in_function){//Vloží se do local
			if(idcko != NULL){
				if(idcko->nodeType == nFunction)
					return SEM_ERROR_DEF;
			}

			idcko = symtableSearch(local, pid.attribute.string->string);
			if(idcko != NULL){//Už existuje
			
			}else{//Neexistuje
				if((retval = symtableInsertV(local, pid.attribute.string)))
					return retval;
				//printf("Vkládám proměnnou %s do local %p\n", pid.attribute.string->string, (void *)local);
				idcko = symtableSearch(local, pid.attribute.string->string);
				if(idcko == NULL)
					return INTERNAL_ERROR;
				printf("DEFVAR LF@%s\n", pid.attribute.string->string);
				var_content = idcko->content;
				initVariable(var_content);
			}
		}else{//Vloží se do global
			if(idcko != NULL){//Už existuje
				if(idcko->nodeType == nFunction)
					return SEM_ERROR_DEF;
			}else{//Neexistuje
				if((retval = symtableInsertV(global, pid.attribute.string)))
					return retval;
				//printf("Vkládám proměnnou %s do global\n", pid.attribute.string->string);
				idcko = symtableSearch(global, pid.attribute.string->string);
				if(idcko == NULL)
					return INTERNAL_ERROR;
				printf("DEFVAR GF@%s\n", pid.attribute.string->string);
				var_content = idcko->content;
				initVariable(var_content);
				
			}
		}
		char* lastidcko = malloc(pid.attribute.string->length);
		strcpy(lastidcko, pid.attribute.string->string);
		GET_TOKEN();
		if((retval = rovn(&pid, lastidcko)))
			return retval;
		free(lastidcko);
		//Nějak zpracovat typ proměnné
		
		return SYNTAX_OK;	
	}
    return SYNTAX_ERROR;
}

static int rovn(Token* pid, char* lastid){
    int retval;
    //printf("---in rovn\n");

    if(token.type == TOKEN_ID){
        //23. <rovn> -> id(<arg>)
	//Token first = token;
	GET_TOKEN();
	if(token.type == TOKEN_L_BRACKET){
		//printf("Nazev fce - %s\n", pid->attribute.string->string);
			
		char* fce = malloc(pid->attribute.string->length);
		strcpy(fce, pid->attribute.string->string);

		tBSTNodePtr funkce = symtableSearch(global, pid->attribute.string->string);
		tInsideFunction* fce_content;

		if(funkce == NULL){//Funkce není definovaná
			if((retval = symtableInsertF(global, pid->attribute.string)))
				return retval;
			tBSTNodePtr funkce = symtableSearch(global, pid->attribute.string->string);
			if(funkce == NULL)
				return INTERNAL_ERROR;

			fce_content = funkce->content;
			fce_content->declared = true;
			fce_content->defined = false;
			not_defined += 1;
		}else if(funkce->nodeType == nFunction){//Funkce je definovaná
			fce_content = funkce->content;
		}else{
			return SEM_ERROR_DEF;		
		}
		int param2 = fce_content->parameters;
		int param = fce_content->parameters;
		if((!fce_content->defined)){
			param = 0;
		}
		
		
		GET_TOKEN();
		//Výpis
		printf("#Tvorba framu pro %s\n", pid->attribute.string->string);
		printf("CREATEFRAME\n");
		int count = 1;
		if((retval= arg(&param, fce_content->defined, &count)))
			return retval;

		//Kontrola počtu parametrů
		if(fce_content->defined){
			if(param != 0)
				return SEM_ERROR_PARAM;
		}else{
			if(param2 == param || param2 == 0)
				fce_content->parameters = param;
		}
		
		if(token.type != TOKEN_R_BRACKET)
			return SYNTAX_ERROR;

		printf("CALL !%s\n", fce);
		if(in_function)
			printf("MOVE LF@%s TF@%%retval\n", lastid);
		else		
			printf("MOVE GF@%s TF@%%retval\n", lastid);
		free(fce);
		GET_TOKEN();
    	}else{
		CHECK_PRECEDENCE(token);
	}
    }else{
    	//21. <rovn> -> <expr>
    	GET_TOKEN();
    	CHECK_PRECEDENCE(token);
    }
	
	tBSTNodePtr idcko = NULL;
	if(in_function)
		idcko = symtableSearch(local, lastid);	
	else
		idcko = symtableSearch(global, lastid);
	
	tInsideVariable *var_content;
	if(idcko != NULL)//Musí existovat
		var_content = idcko->content;
	else
		return INTERNAL_ERROR;

	if(token.attribute.keyword == RET_STRING){//Je to string
		free(var_content->string->string);
		d_string_add_string(token.attribute.string, var_content->string);
		var_content->dataType = 3;
	}else if(token.attribute.keyword == RET_INT){//Je to int
		var_content->integer = token.attribute.integer;
		var_content->flt = 0;
		var_content->dataType = 1;
	}else if(token.attribute.keyword == RET_FLOAT){//Je to float
		var_content->integer = 0;
		var_content->flt = token.attribute.flt;
		var_content->dataType = 2;
	}
    return SYNTAX_OK;
}

static int value(){
    if(token.type == TOKEN_INTEGER){
        //24. <value> -> int_value
	printf("int@%d\n", token.attribute.integer);
        return SYNTAX_OK;
    }else if(token.type == TOKEN_STRING){
        //26. <value> -> str_value
	printf("string@");
	for (int i = 0; i < token.attribute.string->length; i++)
	{
		char c = token.attribute.string->string[i];
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
        return SYNTAX_OK;
    }else if(token.type == TOKEN_FLOAT){
        //25. <value> -> float_value
	printf("float@%a\n", token.attribute.flt);
        return SYNTAX_OK;
    }else if(token.type == TOKEN_ID){
	if(in_function){
		tBSTNodePtr funkce = symtableSearch(local, token.attribute.string->string);
		if(funkce == NULL){
			tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
			if(funkce == NULL)
				return SEM_ERROR_DEF;
			printf("GF@");
		}else{
			printf("LF@");		
		}
	}else{
		tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
		if(funkce == NULL)
			return SEM_ERROR_DEF;
		printf("GF@");
	}
	printf("%s\n", token.attribute.string->string);
	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

static int arg(int* param, bool sub, int* count){
	//printf("---in arg\n");
    //29. <arg> -> <value> <arg_n>
	if(token.type == TOKEN_FLOAT || token.type == TOKEN_STRING || token.type == TOKEN_INTEGER || token.type == TOKEN_ID){
		int retval;
		printf("DEFVAR TF@%%%d\n", (*count));
		printf("MOVE TF@%%%d ", (*count)++);
	    	if((retval= value()))
			return retval;
		if(sub)
			(*param) -= 1;
		else
			(*param) += 1;

	    	GET_TOKEN();

	    	return arg_n(param, sub, count);
	}else if(token.type == TOKEN_R_BRACKET){
		//28. <arg> -> ε
		return SYNTAX_OK;	
	}
	return SYNTAX_ERROR;
}

static int arg_n(int* param, bool sub, int* count){
	int retval;
	//printf("---in arg_n \n");
    if(token.type == TOKEN_COMMA){
        //31. <arg_n> -> , <value> <arg-n>
	printf("DEFVAR TF@%%%d\n", (*count));
	printf("MOVE TF@%%%d ", (*count)++);
        GET_TOKEN_CHECK_RULE(value);

	if(sub)
		(*param) -= 1;
	else
		(*param) += 1;

        GET_TOKEN();

        return arg_n(param, sub, count);

    }else if(token.type == TOKEN_R_BRACKET){
	//30 <arg_n> -> ε
    	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

int initVestFunctions(){
	int retval;
	INSERT_VEST_FUNCTION("inputs", 6); //0 paramu
	INSERT_VEST_FUNCTION("inputf", 6); //0 pramu
	INSERT_VEST_FUNCTION("inputi", 6); //0 pramu
	INSERT_VEST_FUNCTION("print", 5); //n paramů
	INSERT_VEST_FUNCTION("len", 3); //1 param
	INSERT_VEST_FUNCTION("substr", 6); //3 pramy
	INSERT_VEST_FUNCTION("ord", 3); //2 paramy
	INSERT_VEST_FUNCTION("chr", 3); //1 param
	return 0;
}

int insertVestFunction(char* s, int leng){
	int retval;

	Dynamic_string strvest;
	d_string_init(&strvest);
	strvest.string = s;
	strvest.length = leng;

	if((retval = symtableInsertF(global, &strvest)))
		return retval;
	tBSTNodePtr funkce = symtableSearch(global, s);
	if(funkce == NULL)
		return INTERNAL_ERROR;

	tInsideFunction* fce_content;
	fce_content = funkce->content;	
	fce_content->declared = true;
	fce_content->defined = true;
	
	return 0;
}

void initVariable(tInsideVariable *content){
	Dynamic_string* str = malloc(sizeof(Dynamic_string));
	d_string_init(str);
	content->string = str;
	content->integer = 0;
	content->flt = 0.0;
	content->dataType = 0;
}


int main(){
	int retval;
	
	global = malloc(sizeof(tSymtable));
	symtableInit(global);
	
	Dynamic_string str;
	d_string_init(&str);
	set_d_string(&str);

	Dynamic_string str2;
	d_string_init(&str2);
	token.attribute.string = &str2;

	Simple_stack stack;
	set_stack(&stack);

	if((retval = initVestFunctions()))
		return retval;

	FILE* file;	
	if((file = fopen("./code/test2", "r"))){}
	set_file(file);

	printf(".IFJcode19\n");
	//Vložení vestavěných funkcí
	printf("LABEL $$main\n");
	
	GET_TOKEN();
	int cont = prog();
	//printf("%d\n", cont);
	
	symtableDispose(global);
	/*symtableDispose(local);*/
	printf("#Konec souboru\n");
	return cont;
}














