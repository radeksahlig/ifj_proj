#include <stdlib.h>

#include "error.h"
#include "scanner.h"
#include "precedent.h"
#include "analyse.h"
#include "symtable.h"

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

#define GET_TOKEN_CHECK_PRECEDENCE() do{\
    GET_TOKEN();\
    if((retval = precedent_analys(&token)))\
        return retval;\
}while(0)

#define GET_TOKEN() do{\
	if((retval = new_token(&token)))\
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
		printf("DEDENT OK\n");\
		GET_TOKEN();\
		return SYNTAX_OK;\
	    }\
	    GET_TOKEN();\
        }\
        if((prev_indent) == (base_indent)){\
	    printf("DEDENT OK\n");\
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
	printf("---in prog\n");
	int retval;
    	int cur_indent = 0;
    	int prev_indent = 0;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_DEF){
		//1. <prog> -> def id (<params>) : EOL <stmt> EOL <prog>
		printf("---in def\n");

		GET_TOKEN_CHECK_TYPE(TOKEN_ID);

		in_function = true;
		bool was_declared = false;
		int param = 0;
		printf("%s -- funkce \n",token.attribute.string->string);

		tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
		tInsideFunction* fce_content;

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
			printf("Vkládám funkci do global -> %s\n", token.attribute.string->string);
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

		printf("Funkce %s má %d(%d) parametrů\n", token.attribute.string->string, fce_content->parameters, param);

		if((param != fce_content->parameters) && was_declared)
			return SEM_ERROR_PARAM;

		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, prev_indent);

		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;

		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		in_function = false;
		return prog();

	}else if(token.type == TOKEN_EOL){
		printf("---in eol prog\n");
		//2. <prog> -> EOL <prog>
		GET_TOKEN();
		return prog();
	}else if(token.type == TOKEN_EOF){
		printf("---in eof -- nedefinovaných fcí je : %d\n", not_defined);
		//3. <prog> -> EOF
		//Projít nezavolaný fce
		if(not_defined == 0)
			return SYNTAX_OK;
		return SEM_ERROR_DEF;
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		printf("---in if prog\n");        
		//4. <prog> -> if <expr> : EOL <stmt> else : EOL <stmt> <prog>
		GET_TOKEN_CHECK_PRECEDENCE();
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		printf("---in else prog\n");
		if(token.type != TOKEN_KEYWORD || token.attribute.keyword != KEYWORD_ELSE)
			return SYNTAX_ERROR;  
		//GET_TOKEN_CHECK_KEYWORD(KEYWORD_ELSE);
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		printf("---in while prog\n");
		//5. <prog> -> while <expr> : EOL <stmt> EOL <prog>
		GET_TOKEN_CHECK_PRECEDENCE();
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		return prog();
	}else if(token.type == TOKEN_ID){
		printf("---in id prog\n");
		//6. <prog> -> id<def> EOL <prog>
		if((retval = def()))
			return retval;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){
		printf("---in pass prog\n");        
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
	printf("---in stmt indent : %d - %d token\n", prev_indent, cur_indent);
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		//8. <stmt> ->  if <expr> : EOL <stmt> else : EOL <stmt> EOL <stmt>
		printf("---in if stmt\n"); 
		GET_TOKEN_CHECK_PRECEDENCE();
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;
		printf("---in else stmt\n");
	if(token.type != TOKEN_KEYWORD || token.attribute.keyword != KEYWORD_ELSE)
		return SYNTAX_ERROR;  
		//GET_TOKEN_CHECK_KEYWORD(KEYWORD_ELSE);
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;
		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		printf("---in while stmt\n"); 
		//9. <stmt> ->  while <expr> : EOL <stmt> EOL <stmt>
		GET_TOKEN_CHECK_PRECEDENCE();
		GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;
		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_ID){
		printf("---in id stmt\n"); 
		//11. <stmt> -> id <def> EOL <stmt> 
		if((retval = def()))
			return retval;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		GET_TOKEN();
        	return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){
		printf("---in pass stmt\n");         
		//13. <stmt> -> pass EOL <stmt>
	        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
	        GET_TOKEN();
	        return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_EOL){
		printf("---in eol stmt\n"); 
		//10. <stmt> -> EOL <stmt>
		GET_TOKEN();
		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_RETURN){
		printf("---in return stmt\n"); 
		//12. <stmt> -> return <expr> EOL <stmt>
		GET_TOKEN();
		if(token.type == TOKEN_EOL){
			//return None;
		}else{
			if((retval = precedent_analys(&token)))
				return retval;
			GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
			// return vyraz
		}
		CHECK_NUM_DEDENT(cur_indent, prev_indent);
	}
	if(token.type == TOKEN_DEDENT){
	printf("---in dedent stmt %d - %d token\n", prev_indent, cur_indent);
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
    }
	printf("---in stmt end\n"); 
    //14. <stmt> -> ε
	//není return -> retval = None
	return SYNTAX_OK;
}

static int params(tInsideFunction* funkce){
	int retval;
	printf("---in params\n");
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
	printf("Vkládám proměnnou %s do local %p\n", token.attribute.string->string, (void *)local);
	idcko = symtableSearch(local, token.attribute.string->string);
	if(idcko == NULL)
		return INTERNAL_ERROR;

	funkce->paramName[funkce->parameters] = token.attribute.string->string;
	funkce->parameters += 1;
	printf("Přidám parametr %d s nazvem %s\n", funkce->parameters, token.attribute.string->string);
	GET_TOKEN();
        return param_n(funkce);
	}
    //15. <params> -> ε
	return SYNTAX_OK;
}

static int param_n(tInsideFunction* funkce){
	int retval;
	printf("---in param_n\n");
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
		printf("Vkládám proměnnou %s do local %p\n", token.attribute.string->string, (void *)local);
		idcko = symtableSearch(local, token.attribute.string->string);
		if(idcko == NULL)
			return INTERNAL_ERROR;

		funkce->paramName[funkce->parameters] = token.attribute.string->string;
		funkce->parameters += 1;
		printf("Přidám parametr %d s nazvem %s\n", funkce->parameters, token.attribute.string->string);
		GET_TOKEN();

		return param_n(funkce);	
	}else if(token.type == TOKEN_R_BRACKET)
		return SYNTAX_OK;
    return SYNTAX_ERROR;
}

static int def(){
	int retval;
	printf("---in def\n");

	Token pid = token;

	GET_TOKEN();

	if(token.type == TOKEN_L_BRACKET){
		//19. <def> -> (<arg>)
		printf("Nazev fce - %s\n", pid.attribute.string->string);
			
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
		
		if((retval= arg(&param, fce_content->defined)))
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


		return SYNTAX_OK;
	}else if(token.type == TOKEN_ASSIGN){
		//20. <def> -> = <rovn>
		tBSTNodePtr idcko = symtableSearch(global, pid.attribute.string->string);
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
				printf("Vkládám proměnnou %s do local %p\n", pid.attribute.string->string, (void *)local);
				idcko = symtableSearch(local, pid.attribute.string->string);
				if(idcko == NULL)
					return INTERNAL_ERROR;
			}
		}else{//Vloží se do global
			if(idcko != NULL){//Už existuje
				if(idcko->nodeType == nFunction)
					return SEM_ERROR_DEF;
			}else{//Neexistuje
				if((retval = symtableInsertV(global, pid.attribute.string)))
					return retval;
				printf("Vkládám proměnnou %s do global\n", pid.attribute.string->string);
				idcko = symtableSearch(global, pid.attribute.string->string);
				if(idcko == NULL)
					return INTERNAL_ERROR;
			}
		}

		GET_TOKEN();
		
		if((retval = rovn(&pid)))
			return retval;

		//Nějak zpracovat typ proměnné
		
		return SYNTAX_OK;	
	}
    return SYNTAX_ERROR;
}

static int rovn(Token* pid){
    int retval;
    printf("---in rovn\n");

    if(token.type == TOKEN_ID){
        //23. <rovn> -> id(<arg>)
        //kontrola proměnné
	GET_TOKEN();
	if(token.type == TOKEN_L_BRACKET){
		printf("Nazev fce - %s\n", pid->attribute.string->string);
			
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
		
		if((retval= arg(&param, fce_content->defined)))
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


		return SYNTAX_OK;
    	}else{
		return precedent_analys(&token);	
	}
    }
    //21. <rovn> -> <expr>
    GET_TOKEN();
    return precedent_analys(&token);
}

static int value(){
	printf("---in value\n");
    if(token.type == TOKEN_INTEGER){
        //24. <value> -> int_value
        return SYNTAX_OK;
    }else if(token.type == TOKEN_STRING){
        //26. <value> -> str_value
        return SYNTAX_OK;
    }else if(token.type == TOKEN_FLOAT){
        //25. <value> -> float_value
        return SYNTAX_OK;
    }else if(token.type == TOKEN_ID){
	if(in_function){
		tBSTNodePtr funkce = symtableSearch(local, token.attribute.string->string);
		if(funkce == NULL){
			tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
			if(funkce == NULL)
				return SEM_ERROR_DEF;
		}
	}else{
		tBSTNodePtr funkce = symtableSearch(global, token.attribute.string->string);
		if(funkce == NULL)
			return SEM_ERROR_DEF;
	}
	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

static int arg(int* param, bool sub){
	printf("---in arg\n");
    //29. <arg> -> <value> <arg_n>
	if(token.type == TOKEN_FLOAT || token.type == TOKEN_STRING || token.type == TOKEN_INTEGER || token.type == TOKEN_ID){
		int retval;

	    	if((retval= value()))
			return retval;

		if(sub)
			(*param) -= 1;
		else
			(*param) += 1;

	    	GET_TOKEN();

	    	return arg_n(param, sub);
	}else if(token.type == TOKEN_R_BRACKET){
		//28. <arg> -> ε
		return SYNTAX_OK;	
	}
	return SYNTAX_ERROR;
}

static int arg_n(int* param, bool sub){
	int retval;
	printf("---in arg_n \n");
    if(token.type == TOKEN_COMMA){
        //31. <arg_n> -> , <value> <arg-n>
        GET_TOKEN_CHECK_RULE(value);

	if(sub)
		(*param) -= 1;
	else
		(*param) += 1;

        GET_TOKEN();

        return arg_n(param, sub);

    }else if(token.type == TOKEN_R_BRACKET){
	//30 <arg_n> -> ε
    	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

int initVestFunctions(){
	int retval;
	INSERT_VEST_FUNCTION("inputs", 6);
	INSERT_VEST_FUNCTION("inputf", 6);
	INSERT_VEST_FUNCTION("inputi", 6);

	return 0;
}

int insertVestFunction(char* s, int leng){
	int retval;

	Dynamic_string strvest;
	d_string_init(&strvest);
	strvest.string = s;
	strvest.length = leng;

	printf("Vkládám funkci do global -> %s\n", s);
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
	if((file = fopen("./code/test2", "r")))
		printf("Soubor otevřen");
	set_file(file);
	if((retval = new_token(&token)))
		return retval;
	int cont = prog();
	printf("%d\n", cont);
	
	symtableDispose(global);
	/*symtableDispose(local);*/
	return cont;
}














