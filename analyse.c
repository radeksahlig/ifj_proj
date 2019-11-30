#include <stdlib.h>

#include "error.h"
#include "scanner.h"

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

#define SYNTAX_OK 0
#define SYTNAX_ERROR 0

Token token;

static int prog();
static int stmt(int prev_indent, int cur_indent);
static int params();
static int param_n();
static int def();
static int rovn();
static int value();
static int arg();
static int arg_n();
int precedent_analys();


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
	}
	if(token->type == TOKEN_STRING){
		printf(" ## %s ##", token->attribute.string->string);	
	}
	if(retval != 0){
		printf("%d return new_token\n", retval);
		return retval;}
	return 0;
}
int i, j;
int prog(){
	printf("---in prog\n");
	int retval;
    	int cur_indent = 0;
    	int prev_indent = 0;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_DEF){
        //1. <prog> -> def id (<params>) : EOL <stmt> EOL <prog>
	printf("---in def\n");
        GET_TOKEN_CHECK_TYPE(TOKEN_ID);
        //proměnná
        GET_TOKEN_CHECK_TYPE(TOKEN_L_BRACKET);
        GET_TOKEN_CHECK_RULE(params);
        GET_TOKEN_CHECK_TYPE(TOKEN_R_BRACKET);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, prev_indent);
        CHECK_STMT(prev_indent, cur_indent);
        cur_indent = 0;
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        return prog();
	}else if(token.type == TOKEN_EOL){
	printf("---in eol prog\n");
        //2. <prog> -> EOL <prog>
	GET_TOKEN();
        return prog();
	}else if(token.type == TOKEN_EOF){
	printf("---in eof\n");
        //3. <prog> -> EOF
        return SYNTAX_OK;
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
	printf("---in if prog\n");        
	//4. <prog> -> if <expr> : EOL <stmt> else : EOL <stmt> <prog>
        GET_TOKEN_CHECK_RULE(precedent_analys);
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
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, prev_indent);
        CHECK_STMT(prev_indent, cur_indent);
	cur_indent = 0;
        return prog();
	}else if(token.type == TOKEN_ID){
	printf("---in id prog\n");
        //6. <prog> -> id<def> EOL <prog>
        GET_TOKEN_CHECK_RULE(def);
	printf("prošlo id ---\n");
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
        GET_TOKEN_CHECK_RULE(precedent_analys);
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
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, this_indent);
        CHECK_STMT(this_indent, cur_indent);
	cur_indent = this_indent;
        return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_ID){
	printf("---in id stmt\n"); 
        //11. <stmt> -> id <def> EOL <stmt> 
        GET_TOKEN_CHECK_RULE(def);
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
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
	}
	if(token.type == TOKEN_DEDENT){
	printf("---in dedent stmt %d - %d token\n", prev_indent, cur_indent);
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
    }
	printf("---in stmt end\n"); 
    //14. <stmt> -> ε
	return SYNTAX_OK;
}

static int params(){
	printf("---in params\n");
	if(token.type == TOKEN_ID){
        //16. <params> -> id <param_n>;
        //nějaká kontrola
        return param_n();
	}
    //15. <params> -> ε
	return SYNTAX_OK;
}

static int param_n(){
	int retval;
	printf("---in param_n\n");
	if(token.type == TOKEN_COMMA){
        //18. <param_n> -> , id <param_n>
        GET_TOKEN_CHECK_TYPE(TOKEN_ID);
	}
    //17. <param_n> -> ε
	return SYNTAX_OK;
}

static int def(){
	int retval;
	printf("---in def\n");
	if(token.type == TOKEN_L_BRACKET){
        //19. <def> -> (<arg>)
        GET_TOKEN_CHECK_RULE(arg);
        if(token.type != TOKEN_R_BRACKET)
        	return SYNTAX_ERROR;
        return SYNTAX_OK;
	}else if(token.type == TOKEN_ASSIGN){
        //20. <def> -> = <rovn>
        GET_TOKEN();
        return rovn();
        //nějak ukládat data do proměnné
	}
    return SYNTAX_ERROR;
}

static int rovn(){
	int retval;
	printf("---in rovn\n");
    if(token.type == TOKEN_ID){
        //23. <rovn> -> id(<arg>)
        //kontrola proměnné
        GET_TOKEN_CHECK_TYPE(TOKEN_L_BRACKET);
        GET_TOKEN_CHECK_RULE(arg);
        if(token.type != TOKEN_R_BRACKET)
        	return SYNTAX_ERROR;
        return SYNTAX_OK;
    }else if(token.type == TOKEN_STRING){
        //22. <rovn> -> str_value !!!!!!!!!!!!!!!!!!!!!!!§asi tu bejt nemá!!!!!!!!!
        return SYNTAX_OK;
    }
    //21. <rovn> -> <expr>
    return precedent_analys();
}

static int value(){
	printf("---in value\n");
    if(token.type == TOKEN_INTEGER){
        //24. <value> -> int_value
        return SYNTAX_OK;
    }else if(token.type == TOKEN_STRING){
        //26. <value> -> str_value
	printf("value string\n");
        return SYNTAX_OK;
    }else if(token.type == TOKEN_FLOAT){
        //25. <value> -> float_value
        return SYNTAX_OK;
    }else if(token.type == TOKEN_ID){
        //27. <value> -> id
        //kontrola proměnné
	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

static int arg(){
	printf("---in arg\n");
        
    //29. <arg> -> <value> <arg_n>
	if(token.type == TOKEN_ID || token.type == TOKEN_FLOAT || token.type == TOKEN_STRING || token.type == TOKEN_INTEGER){
		int retval;
	    	if((retval= value())){
			return retval;
		}
	    	GET_TOKEN();
		printf("---in arg %d\n", i);	
	    	return arg_n();
	}else if(token.type == TOKEN_R_BRACKET){
		//28. <arg> -> ε
		return SYNTAX_OK;	
	}
	return SYNTAX_ERROR;
}

static int arg_n(){
	int retval;
	printf("---in arg_n %d\n", i);
    if(token.type == TOKEN_COMMA){
        //31. <arg_n> -> , <value> <arg-n>
        GET_TOKEN_CHECK_RULE(value);
        GET_TOKEN();
        return arg_n();
    }else if(token.type == TOKEN_R_BRACKET){
	//30 <arg_n> -> ε
    	return SYNTAX_OK;
	}
    return SYNTAX_ERROR;
}

int precedent_analys(){
	printf("---in expr\n");
	return SYNTAX_OK;
}

int main(){
	int retval;
	
	Simple_stack stack;
	set_stack(&stack);

	Dynamic_string str;
	d_string_init(&str);
	set_d_string(&str);

	

	FILE* file;	
	if((file = fopen("./code/test1", "r")))
		printf("Soubor otevřen");
	set_file(file);
	if((retval = new_token(&token)))
		return retval;
	int cont = prog();
	printf("%d\n", cont);	
	return cont;
}














