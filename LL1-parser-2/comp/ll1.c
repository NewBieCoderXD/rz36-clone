// LL1.c    parser for summer 2018
//    grammar from lecture parsing slide page 25

#include "compile.h"

#define Gexp	1		// non-terminal symbols
#define Gexp2	2
#define Gaddop	3
#define Gterm	4
#define Gterm2	5
#define Gmulop	6
#define Gfactor	7

#define Tlparen	20		// terminal symbols
#define Trparen	21
#define Tplus	22
#define Tminus	23
#define Tstar	24
#define Tn		25
#define Teol	26
#define Terror	27

int pTable[][7] = {		// parsing table
	{1,0,0,0,0,1,0},	// exp
	{0,3,2,2,0,0,3}, 	// exp2
	{0,0,4,5,0,0,0},	// addop
	{6,0,0,0,0,6,0},	// term
	{0,8,8,8,7,0,8},	// term2
	{0,0,0,0,9,0,0},	// mulop
	{10,0,0,0,0,11,0}	// factor
};

int pstack[100], sp = 0;	// parser stack
int cinput;				// current input

void error(void){
	printf("error\n");
	exit(0);
}

int convert(int t){
	switch(t){
		case tkLPAREN: return Tlparen;
		case tkRPAREN: return Trparen;
		case tkPLUS:   return Tplus;
		case tkMINUS:  return Tminus;
		case tkSTAR:   return Tstar;
		case tkSEMI:   return Teol;
		case tkIDEN:   return Tn;
	}
	return Terror;
}

// ----------- print out for display and debugging -----------

int isterminal(int x){
	return Tlparen <= x && x  <= Teol;
}

int isrule(int x){
	return Gexp <= x && x <= Gfactor;
}

static char tstring[][5] = {
	"(", ")", "+", "-", "*", "n", "eol", "err", 0
};

void printterminal(int x){
	printf("%s ",tstring[x-20]);  // convert x to index
}

static char gstring[][8] = {
	"exp", "exp2", "addop", "term", "term2", "mulop", "factor", 0
};


void printitem(int x){		// print item in parser stack
	if( isterminal(x) ){
		printterminal(x);
	}else{
		printf("%s ", gstring[x-1]);
	}
}

void printstack(void){		// print parser stack
	int i;
	printf("<<");
	for(i = sp; i > 0; i--){
		printitem(pstack[i]);
//		printf("%d ",pstack[i]);
	}
	printf(">>\n");
}

// ------------------------------

void getToken(void){
	mylex();
	if(tok == tkEOF) printf("eof ");
	else{
		cinput = convert(tok);
		printterminal(cinput);
	}
}

void push(int x){
	sp++;
	pstack[sp] = x;
}

void pop(void){
	sp--;
}

int tos(void){
	return pstack[sp];
}


int lookup(int rule, int input){
//	printf("lookup rule %d input %d\n", rule, input);
	if( ! isrule(rule) ) error();
	if( ! isterminal(input) ) error();
	return pTable[rule-1][input-20];		// translate to pTable index
}

// ------------- action of derivation --------

void exp(void){
	pop();
	push(Gexp2);
	push(Gterm);
}

void exp2(void){
	pop();
	push(Gexp2);
	push(Gterm);
	push(Gaddop);
}

void term(void){
	pop();
	push(Gterm2);
	push(Gfactor);
}

void term2(void){
	pop();
	push(Gterm2);
	push(Gfactor);
	push(Gmulop);
}

void factor(void){
	pop();
	push(Trparen);
	push(Gexp);
	push(Tlparen);
}

void initLL1(void){
	push(Teol);
	push(Gexp);		// starting symbol
	getToken();		// get first input
}

int parse(void){
	int x;

	while( tos() != Teol ){
		printstack();
		if( cinput == Terror ) error();
		if( isterminal(tos()) ){
			pop();
			getToken();		// advance input
		}else{
			x = lookup(tos(),cinput);
			printf("[%d] ",x);
			switch(x){
			case 0: error(); break;
			case 1: exp(); break;
			case 2: exp2(); break;
			case 3: pop(); break;
			case 4: pop(); push(Tplus); break;
			case 5: pop(); push(Tminus); break;
			case 6: term(); break;
			case 7: term2(); break;
			case 8: pop(); break;
			case 9: pop(); push(Tstar); break;
			case 10: factor(); break;
			case 11: pop(); push(Tn); break;
			}
		}
	}
	return 1;
}


