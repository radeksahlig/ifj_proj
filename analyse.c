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

int prog(){
	int retval;
    	int cur_indent = 0;
    	int prev_indent = 0;
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_DEF){
        //1. <prog> -> def id (<params>) : EOL <stmt> EOL <prog>
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
        //2. <prog> -> EOL <prog>
	GET_TOKEN();
        return prog();
	}else if(token.type == TOKEN_EOF){
        //3. <prog> -> EOF
        return SYNTAX_OK;
	}else if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){    
	//4. <prog> -> if <expr> : EOL <stmt> else : EOL <stmt> <prog>
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, prev_indent);
        CHECK_STMT(prev_indent, cur_indent);
	cur_indent = 0;
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
        //5. <prog> -> while <expr> : EOL <stmt> EOL <prog>
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, prev_indent);
        CHECK_STMT(prev_indent, cur_indent);
	cur_indent = 0;
        return prog();
	}else if(token.type == TOKEN_ID){
        //6. <prog> -> id<def> EOL <prog>
        GET_TOKEN_CHECK_RULE(def);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
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
	if(token.type == TOKEN_KEYWORD && token.attribute.keyword == KEYWORD_IF){
        //8. <stmt> ->  if <expr> : EOL <stmt> else : EOL <stmt> EOL <stmt>
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, this_indent);
        CHECK_STMT(this_indent, cur_indent);
	cur_indent = this_indent;
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
        //9. <stmt> ->  while <expr> : EOL <stmt> EOL <stmt>
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_INDENT(cur_indent, this_indent);
        CHECK_STMT(this_indent, cur_indent);
	cur_indent = this_indent;
        return stmt(prev_indent, cur_indent);
	}else if(token.type == TOKEN_ID){
        //11. <stmt> -> id <def> EOL <stmt> 
        GET_TOKEN_CHECK_RULE(def);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
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
        GET_TOKEN_CHECK_RULE(precedent_analys);
        GET_TOKEN_CHECK_TYPE(TOKEN_EOL);
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
	}
	if(token.type == TOKEN_DEDENT){
        CHECK_NUM_DEDENT(cur_indent, prev_indent);
    }
	printf("---in stmt end\n"); 
    //14. <stmt> -> ε
	return SYNTAX_OK;
}

static int params(){
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
	if(token.type == TOKEN_COMMA){
        //18. <param_n> -> , id <param_n>
        GET_TOKEN_CHECK_TYPE(TOKEN_ID);
	}
    //17. <param_n> -> ε
	return SYNTAX_OK;
}

static int def(){
	int retval;
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
        //27. <value> -> id
        //kontrola proměnné
	return SYNTAX_OK;
    }
    return SYNTAX_ERROR;
}

static int arg(){
        
    //29. <arg> -> <value> <arg_n>
	if(token.type == TOKEN_ID || token.type == TOKEN_FLOAT || token.type == TOKEN_STRING || token.type == TOKEN_INTEGER){
		int retval;
	    	if((retval= value())){
			return retval;
		}
	    	GET_TOKEN();
	    	return arg_n();
	}else if(token.type == TOKEN_R_BRACKET){
		//28. <arg> -> ε
		return SYNTAX_OK;	
	}
	return SYNTAX_ERROR;
}

static int arg_n(){
	int retval;
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
	file = stdin;
	set_file(file);
	GET_TOKEN();
	return prog();
}
