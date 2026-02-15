// asm.c    parser for Tawan asm

#include "compile.h"

#define eqs(s1,s2)    (strcmp(s1,s2) == 0)

void mylex2(void){
	mylex();
	if(tok == 66) printf(", ");
	else if(tok == tkEOF) printf("eof ");
	else printf("%s ",tokstring);
}

int num(void){
	if(tok == 11){ 	// match number token
		mylex2();	// next token
		return 1;
	}
	return 0;
}

int reg(void){
	if(tok != 10) return 0;				// check token type
	if(tokstring[0] != 'R') return 0;	// check 'R'
	mylex2();							// next token
	return 1;
}

int isop(void){
	return eqs(tokstring, "CLRA") || eqs(tokstring, "MOV") ||
	       eqs(tokstring, "ADD")  || eqs(tokstring, "MOVA") ;
}

int op(void){
	if(isop()){		// 	match op token
		mylex2();	//  next token
		return 1;
	}
	return 0;
}

int oprnd(void){
	if(reg() != 0){
		if(tok == 66){		// match ,
			mylex2();		// next token
			return num();
		}else
			return 1;
	}
	return 1;
}

int asm(void){
	if( tok == tkEOF ) return 1;
	if( op() != 0 ){
		oprnd();
		asm();
	}
	return 0;
}


