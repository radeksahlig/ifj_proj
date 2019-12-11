#include <stdio.h>
#include <stdlib.h>

void genVestFce(){
	//Vestavěné funkce
	//inputs()
	printf("#Začátek funkce inputs\n");
	printf("LABEL !inputs\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval string\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce inputs\n");	

	//inputf()
	printf("#Začátek funkce inputf\n");
	printf("LABEL !inputf\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval float\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("#Konec funkce inputf\n");

	//inputi()
	printf("#Začátek funkce inputi\n");
	printf("LABEL !inputi\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("READ LF@%%retval int\n");
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
	printf("DEFVAR LF@$s");
	printf("MOV LF@$s LF@%%1");
	printf("DEFVAR LF@$i");
	printf("MOV LF@$i LF@%%2");
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
	printf("DEFVAR LF@$i");
	printf("MOVE LF@$i LF@%%1");
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

	//Předělat substr !

	//substr()
	printf("#substr()\n");
	printf("LABEL $substr\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval string@\n");
	printf("DEFVAR LF@str\n");
	printf("CREATEFRAME\n");
	printf("DEFVAR TF@%%1\n");
	printf("MOVE TF@%%1 LF@%%1\n");
	printf("CALL !len\n");
	printf("MOVE LF@str TF@%%retval\n");
	printf("DEFVAR LF@return\n");
	printf("EQ LF@return LF@str int@0\n");
	printf("JUMPIFEQ $substrend LF@return bool@true\n");
	printf("LT LF@return LF@str int@0\n");
	printf("JUMPIFEQ $substrend LF@return bool@true\n");
	printf("LT LF@return LF@%%2 int@0\n");
	printf("JUMPIFEQ $substrend LF@return bool@true\n");
	printf("EQ LF@return LF@%%3 int@0\n");
	printf("JUMPIFEQ $substrend LF@return bool@true\n");
	printf("GT LF@return LF@%%2 LF@str\n");
	printf("JUMPIFEQ $substrend LF@return bool@true\n");
	printf("DEFVAR LF@max\n");
	printf("MOVE LF@max LF@str\n");
	printf("ADD LF@max LF@max int@1\n");
	printf("SUB LF@max LF@max LF@%%2\n");
	printf("DEFVAR LF@index\n");
	printf("MOVE LF@index int@0\n");
	printf("DEFVAR LF@edit\n");
	printf("GT LF@edit LF@%%3 LF@max\n");
	printf("JUMPIFEQ $substredit LF@edit bool@true\n");
	printf("LT LF@edit LF@%%3 int@0\n");
	printf("JUMPIFEQ $substredit LF@edit bool@true\n");
	printf("JUMP $substrproces\n");
	printf("LABEL $substredit\n");
	printf("MOVE LF@%%3 LF@max\n");
	printf("LABEL $substrproces\n");
	printf("CREATEFRAME\n");
	printf("DEFVAR TF@char\n");
	printf("DEFVAR TF@loop\n");
	printf("GETCHAR TF@char LF@%%1 LF@%%2\n");
	printf("ADD LF@%%2 LF@%%2 int@1\n");
	printf("CONCAT LF@%%retval LF@%%retval TF@char\n");
	printf("ADD LF@index LF@index int@1\n");
	printf("GT TF@loop LF@%%3 LF@index\n");
	printf("JUMPIFEQ $substrproces TF@loop bool@true\n");
	printf("LABEL $substrend\n");
	printf("POPFRAME\n");
	printf("RETURN\n");

}
