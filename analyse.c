/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  analyse.c                                        *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

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

#define CHECK_PRECEDENCE(tok) do{\
	if((retval = precedent_analys(tok)))\
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

#define INSERT_VEST_FUNCTION(s, l, p)do{\
	if((retval = insertVestFunction(s, l, p)))\
		return retval;\
}while(0)

Token token;
tSymtable* global;
tSymtable* local;
int not_defined = 0;
bool in_function = false;
int global_label = 1;


int prog(){
	int retval;
    	int cur_indent = 0;
    	int prev_indent = 0;
	int label = global_label++;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_DEF){
		//1. <prog> -> def id (<params>) : EOL <stmt> EOL <prog>

		GET_TOKEN_CHECK_TYPE(TOKEN_ID);

		in_function = true;
		setInFunction(true);
		bool was_declared = false;
		int param = 0;
		
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
				setLocTable(local);
				fce_content->local = local;
			}
		}else{//Funkce se vloží do symtabl
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
			setLocTable(local);
			fce_content->local = local;
		}
		

		GET_TOKEN_CHECK_TYPE(TOKEN_L_BRACKET);
	
		GET_TOKEN();
		if((retval = params(fce_content)))
			return retval;
		if(token.type != TOKEN_R_BRACKET)
			return SYNTAX_ERROR;
		
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
		in_function = false;
		setInFunction(false);
		return prog();

	}else if(token.type == TOKEN_EOL){
		//2. <prog> -> EOL <prog>
		GET_TOKEN();
		return prog();
	}else if(token.type == TOKEN_EOF){
		//3. <prog> -> EOF
		//Projít nezavolaný fce
		if(not_defined == 0)
			return SYNTAX_OK;
		return SEM_ERROR_DEF;
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		//4. <prog> -> if <expr> : EOL <stmt> else : EOL <stmt> <prog>
		printf("#Začátek ifu v progu\n");

		GET_TOKEN();
		CHECK_PRECEDENCE(NULL);

		printf("DEFVAR GF@$ifprom%d\n", label);
		printf("POPS GF@$ifprom%d\n", label);
		printf("JUMPIFNEQ $else$%d GF@$ifprom%d bool@true\n", label, label);

		if(token.type != TOKEN_COLON)
			return SYNTAX_ERROR;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);

		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;

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
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		//5. <prog> -> while <expr> : EOL <stmt> EOL <prog>

		printf("#Začátek while v progu\n");
		printf("DEFVAR GF@$whileprom%d\n", label);
		printf("LABEL $while$start%d\n", label);
		GET_TOKEN();
		CHECK_PRECEDENCE(NULL);
		
		printf("POPS GF@$whileprom%d\n", label);
		printf("JUMPIFNEQ $while$end%d GF@$whileprom%d bool@true\n", label, label);

		if(token.type != TOKEN_COLON)
			return SYNTAX_ERROR;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
		CHECK_NUM_INDENT(cur_indent, prev_indent);
		CHECK_STMT(prev_indent, cur_indent);
		cur_indent = 0;
		
		printf("JUMP $while$start%d\n", label);
		printf("LABEL $while$end%d\n", label);

		return prog();
	}else if(token.type == TOKEN_ID){
		//6. <prog> -> id<def> EOL <prog>
		if((retval = def()))
			return retval;
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;		
		return prog();
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){    
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
	int label = global_label++;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
		//8. <stmt> ->  if <expr> : EOL <stmt> else : EOL <stmt> EOL <stmt>
		printf("#začátek ifu v stmt\n");

		GET_TOKEN(); 
		CHECK_PRECEDENCE(NULL);

		printf("DEFVAR GF@$ifprom%d\n", label);
		printf("POPS GF@$ifprom%d\n", label);
		printf("JUMPIFNEQ $else$%d GF@$ifprom%d bool@true\n", label, label);
		//podmínka skoku

		if(token.type != TOKEN_COLON)
			return SYNTAX_ERROR;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;

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

		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_WHILE){
		//9. <stmt> ->  while <expr> : EOL <stmt> EOL <stmt>

		printf("#Začátek while v stmt\n");
		printf("DEFVAR GF@$whileprom%d\n", label);
		printf("LABEL $while$start%d\n", label);

		GET_TOKEN();
		CHECK_PRECEDENCE(NULL);

		printf("POPS GF@$whileprom%d\n", label);
		printf("JUMPIFNEQ $while$konec%d GF@$whileprom%d bool@true\n", label, label);
		//Podmíněný skok

		if(token.type != TOKEN_COLON)
			return SYNTAX_ERROR;
		GET_TOKEN_CHECK_TYPE(TOKEN_EOL);

		CHECK_NUM_INDENT(cur_indent, this_indent);
		CHECK_STMT(this_indent, cur_indent);
		cur_indent = this_indent;

		printf("JUMP $while$start%d\n", label);
		printf("LABEL $while$konec%d\n", label);

		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_ID){
		//11. <stmt> -> id <def> EOL <stmt> 
		if((retval = def()))
			return retval;
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;
		GET_TOKEN();
        	return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_PASS){     
		//13. <stmt> -> pass EOL <stmt>
	        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
	        GET_TOKEN();
	        return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_EOL){
		//10. <stmt> -> EOL <stmt>
		GET_TOKEN();
		return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_RETURN){
		//12. <stmt> -> return <expr> EOL <stmt>
		GET_TOKEN();
		CHECK_PRECEDENCE(NULL);
		if(token.type != TOKEN_EOL)
			return SYNTAX_ERROR;
		printf("POPS LF@%%retval\n");
		GET_TOKEN();
		CHECK_NUM_DEDENT(cur_indent, prev_indent);
	}
	if(token.type == TOKEN_DEDENT){
        	CHECK_NUM_DEDENT(cur_indent, prev_indent);
    	}
    	//14. <stmt> -> ε
	//není return -> retval = None
	return SYNTAX_OK;
}

static int params(tInsideFunction* funkce){
	int retval;
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
	idcko = symtableSearch(local, token.attribute.string->string);
	if(idcko == NULL)
		return INTERNAL_ERROR;
		
	initVariable(idcko->content);

	strcpy((char *) funkce->paramName[funkce->parameters],token.attribute.string->string);
	funkce->parameters += 1;
	
	tInsideVariable* content = idcko->content;
	content->dataType = 1;

	GET_TOKEN();
        return param_n(funkce);
	}
    //15. <params> -> ε
	return SYNTAX_OK;
}

static int param_n(tInsideFunction* funkce){
	int retval;
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
	
		idcko = symtableSearch(local, token.attribute.string->string);
		if(idcko == NULL)
			return INTERNAL_ERROR;

		initVariable(idcko->content);

		strcpy((char *)funkce->paramName[funkce->parameters],token.attribute.string->string);
		if(funkce->parameters == 1)
			strcpy(funkce->prvniparam, token.attribute.string->string);
		funkce->parameters += 1;
		
		tInsideVariable* content = idcko->content;
		content->dataType = 1;

		GET_TOKEN();
		return param_n(funkce);	
	}else if(token.type == TOKEN_R_BRACKET)
		return SYNTAX_OK;
    return SYNTAX_ERROR;
}

static int def(){
	int retval;

	Token pid = token;
	GET_TOKEN();

	if(token.type == TOKEN_L_BRACKET){
		//19. <def> -> (<arg>)
		if(strcmp(pid.attribute.string->string,"print") == 0){
			int a = 11;
			GET_TOKEN();
			if((retval= arg(&a, false, &a, "")))
				return retval;
		}else{	
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
			printf("CREATEFRAME\n");

			char* fce = malloc(pid.attribute.string->length);
			strcpy(fce, pid.attribute.string->string);
			int count = 1;
			if((retval= arg(&param, fce_content->defined, &count, fce)))
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
		}
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
     		if(token.type == TOKEN_ID){
			if(strcmp(token.attribute.string->string,"inputi") == 0){
				var_content->dataType = 1;
			}else if(strcmp(token.attribute.string->string,"inputf") == 0){
				var_content->dataType = 2;
			}else if(strcmp(token.attribute.string->string,"inputs") == 0){
				var_content->dataType = 3;
			}else if(var_content->dataType == 0){
				var_content->dataType = 1;
			}	
		}	
		if((retval = rovn(&pid, lastidcko)))
			return retval;
		free(lastidcko);
		
		return SYNTAX_OK;	
	}
    return SYNTAX_ERROR;
}

static int rovn(Token* pid, char* lastid){
    int retval;
    bool mov = false;
    if(token.type == TOKEN_ID){
        //23. <rovn> -> id(<arg>)
	Token* help = malloc(sizeof(Token));
	Dynamic_string str3;
	d_string_init(&str3);
	help->attribute.string = &str3;
	*help = token;
	d_string_add_string(token.attribute.string, help->attribute.string);
	GET_TOKEN();
	if(token.type == TOKEN_L_BRACKET){
			
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
		printf("#Tvorba framu pro %s\n", fce);
		printf("CREATEFRAME\n");
		int count = 1;
		if((retval= arg(&param, fce_content->defined, &count, fce)))
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
		mov = true;
		printf("CALL !%s\n", fce);
		if(in_function)
			printf("MOVE LF@%s TF@%%retval\n", lastid);
		else	
			printf("MOVE GF@%s TF@%%retval\n", lastid);
		free(fce);
		GET_TOKEN();
    	}else{
		CHECK_PRECEDENCE(help);
		
	}
	free(help);
    }else{
    	//21. <rovn> -> <expr>
    	CHECK_PRECEDENCE(NULL);
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
	if(!mov){
		if(in_function){
			if(symtableSearch(local, lastid) != NULL)
				printf("POPS LF@%s\n", lastid);
			else
				printf("POPS GF@%s\n", lastid);	
		}else{
			printf("POPS GF@%s\n", lastid);	
		}
	}

	if(token.attribute.keyword == RET_STRING){//Je to string
		free(var_content->string->string);
		var_content->string->length = 0;
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

static int value(int count, char* fce){
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
	tBSTNodePtr funkce;
	if(in_function){
		funkce = symtableSearch(local, token.attribute.string->string);
		if(funkce == NULL){
			funkce = symtableSearch(global, token.attribute.string->string);
			if(funkce == NULL)
				return SEM_ERROR_DEF;
			printf("GF@");
		}else{
			printf("LF@");		
		}
	}else{
		funkce = symtableSearch(global, token.attribute.string->string);
		if(funkce == NULL)
			return SEM_ERROR_DEF;
		printf("GF@");
	}
	printf("%s\n", token.attribute.string->string);

	if(strcmp(fce, "") != 0){
		tBSTNodePtr prvfce = symtableSearch(global, fce);
		tInsideFunction *content = prvfce->content;
		tBSTNodePtr prom;	
		if(count == 1)
			prom = symtableSearch(content->local, content->prvniparam);
		else
			prom = symtableSearch(content->local, (char *)content->paramName[count]);
		tInsideVariable *cnt = prom->content;
		tInsideVariable *cntprom = funkce->content;
   		cnt->dataType = cntprom->dataType;
	}
	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

static int arg(int* param, bool sub, int* count, char* fce){
    //29. <arg> -> <value> <arg_n>
	if(token.type == TOKEN_FLOAT || token.type == TOKEN_STRING || token.type == TOKEN_INTEGER || token.type == TOKEN_ID){
		int retval;
		if((*param) == 11 && (*count) == 11){
			printf("WRITE ");
			if((retval= value(0, "")))
				return retval;
		}else{
			printf("DEFVAR TF@%%%d\n", (*count));
			printf("MOVE TF@%%%d ", (*count)++);
		    	if((retval= value((*count)-2, fce)))
				return retval;
			if(sub)
				(*param) -= 1;
			else
				(*param) += 1;	
		}
		GET_TOKEN();
	    	return arg_n(param, sub, count, fce);
	}else if(token.type == TOKEN_R_BRACKET){
		//28. <arg> -> ε
		return SYNTAX_OK;	
	}
	return SYNTAX_ERROR;
}

static int arg_n(int* param, bool sub, int* count, char* fce){
	int retval;
    if(token.type == TOKEN_COMMA){
        //31. <arg_n> -> , <value> <arg-n>
	if((*param) == 11 && (*count) == 11){
		printf("WRITE ");
		GET_TOKEN();
		if((retval= value(0, "")))
			return retval;
	}else{
		
		printf("DEFVAR TF@%%%d\n", (*count));
		printf("MOVE TF@%%%d ", (*count)++);
		GET_TOKEN();
		if((retval= value((*count)-2, fce)))
			return retval;
		if(sub)
			(*param) -= 1;
		else
			(*param) += 1;
	}
        GET_TOKEN();

        return arg_n(param, sub, count, fce);

    }else if(token.type == TOKEN_R_BRACKET){
	//30 <arg_n> -> ε
    	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

int initVestFunctions(){
	int retval;
	INSERT_VEST_FUNCTION("inputs", 6, 0); //0 paramu
	INSERT_VEST_FUNCTION("inputf", 6, 0); //0 pramu
	INSERT_VEST_FUNCTION("inputi", 6, 0); //0 pramu
	INSERT_VEST_FUNCTION("print", 5, 10); //n paramů
	INSERT_VEST_FUNCTION("len", 3, 1); //1 param
	INSERT_VEST_FUNCTION("ord", 3, 2); //2 paramy
	INSERT_VEST_FUNCTION("chr", 3, 1); //1 param
	INSERT_VEST_FUNCTION("substr", 6, 3); //3 pramy
	return 0;
}

int insertVestFunction(char* s, int leng, int pocet){
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

	local = malloc(sizeof(tSymtable));
	symtableInit(local);
	fce_content->local = local;
	fce_content->parameters = pocet;
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

	setGlobalVariables(&token, global);

	if((retval = initVestFunctions()))
		return retval;

	FILE* file;	
	file = stdin;
	set_file(file);

	printf(".IFJcode19\n");
	printf("JUMP $$main\n");
	genVestFunctions();
	printf("LABEL $$main\n");
	printf("#Pomocne promenne\n");
  	printf("DEFVAR GF@?AX?\n");
	printf("DEFVAR GF@?BX?\n");
	printf("DEFVAR GF@?CX?\n");

	GET_TOKEN();
	int cont = prog();
	
	symtableDispose(global);

	printf("#Konec souboru\n");
	printf("CLEARS\n");
	printf("EXIT int@0\n");

	return cont;
}

void genVestFunctions(){
	//Vestavěné funkce
	//inputs()
	printf("#Začátek funkce inputs\n");
	printf("LABEL !inputs\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval string\n");
        printf("PUSHS LF@%%retval\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce inputs\n");	

	//inputf()
	printf("#Začátek funkce inputf\n");
	printf("LABEL !inputf\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval float\n");
	printf("PUSHS LF@%%retval\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce inputf\n");

	//inputi()
	printf("#Začátek funkce inputi\n");
	printf("LABEL !inputi\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval int\n");
	printf("PUSHS LF@%%retval\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce inputi\n");

	//len()
	printf("#Začátek funkce len\n");
	printf("LABEL !len\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("STRLEN LF@%%retval LF@%%1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce len\n");

	//ord() s - řetězec, i - pozice
	printf("#Začátek funkce ord\n");
	printf("LABEL !ord\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval int@0\n");
	printf("DEFVAR LF@$s\n");
	printf("MOVE LF@$s LF@%%1\n");
	printf("DEFVAR LF@$i\n");
	printf("MOVE LF@$i LF@%%2\n");
	printf("DEFVAR LF@$podm\n");
	printf("LT LF@$podm LF@$i int@1\n");
	printf("JUMPIFEQ !konec!ord LF@$podm bool@true\n");
	printf("DEFVAR LF@$leng\n");
	printf("CREATEFRAME\n");
	printf("DEFVAR TF@%%1\n");
	printf("MOVE TF@%%1 LF@$s\n");
	printf("CALL !len\n");
	printf("MOVE LF@$leng TF@%%retval\n");
	printf("GT LF@$podm LF@$i LF@$leng\n");
	printf("JUMPIFEQ !konec!ord LF@$podm bool@true\n");
	printf("SUB LF@$i LF@$i int@1\n");
	printf("STRI2INT LF@%%retval LF@$s LF@$i\n");
	printf("LABEL !konec!ord\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce ord\n");

	//chr() i - ascii kod
	printf("#Začátek funkce chr\n");
	printf("LABEL !chr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval nil@nil\n");
	printf("DEFVAR LF@$i\n");
	printf("MOVE LF@$i LF@%%1\n");
	printf("DEFVAR LF@$podm\n");
	printf("GT LF@$podm LF@$i int@-1\n");
	printf("JUMPIFEQ !konec!chr LF@$podm bool@true\n");
	printf("LT LF@$podm LF@$i int@256\n");
	printf("JUMPIFEQ !konec!chr LF@$podm bool@true\n");
	printf("EXIT int@58\n");
	printf("LABEL !konec!chr\n");
	printf("INT2CHAR LF@%%retval LF@%%1\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce chr\n");
}










