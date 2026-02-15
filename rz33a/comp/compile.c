/* compile.c
	P.Chongstitvatana
	19 Aug 97
	1 Sept 97
	3 Apr 2001  start project RZ
	1 Jan 2002  generate s2 code
	27 Sept 2010   begin project rz3, generate parse-tree

*/

#include "compile.h"
//#include "icname.h"

extern int pass(void);		// parse.c
extern int nsym;			// number of symbol in symbol table
extern int CS[];			// code segment
FILE *FO;					// output file

void seterror( char *mess ){
	printf("%s\n",mess);
	exit(1);
}

void strcat2(char *s1, char *s2, char *s3){
	strcpy(s1,s2);
	strcat(s1,s3);
}
/*
void prolog(void){
	int a;
	a = putSym("main",tyFUNCTION,0,0);
	ypush(MARK);
	mylex();
}
*/
// search symbol table for Function and show it
void showParseTree(void){
	int i;
	for(i=1; i<=nsym; i++)
		if(getType(i) == tyFUNCTION){
			printList(getRef(i));
			printf("\n");
		}
}
void showGlobal(void){
	int i, ty;
	for(i=1; i<=nsym; i++){
		ty = getType(i);
		if(ty == tySCALAR || ty == tyVECTOR){
			printf("%s type %d ref %d\n",getName(i),ty,getRef(i));
		}
	}
}

void testlex(void){
	mylex();
	while( tok != tkEOF ){
//		printf("\n");
//		prtoken(tok);
		printf("tok %d",tok);
		if(tok == 66) printf(" tokstring ,\n");
		else printf(" tokstring %s\n", tokstring);
		mylex();
	}
}
/*
extern int CP;

// search symbol table for FUNCTION and generate its code
void genall(void){
	int i;
	for(i=1; i<=nsym; i++)
		if(getType(i) == tyFUNCTION){
			genex(getRef(i));
			printf("\n");
		}
	patchCalli(1,CP-1);
}

void epilog(void){
	int a;
	a = CP;
	CP = 1;
	outa(icCall,getMainRef());
	outs(icEnd);
	CP = a;
}

// move c-string from strbuf to M[] at STRBASE

extern char strbuf[];
extern int freestr;
extern int M[];

void dumpString(void){
	int i;
	for(i=0; i<freestr; i++)
		M[STRBASE+i] = (int)(unsigned char)strbuf[i];
}

// produce object code
void outObj(string fn){
	FO = fopen(fn,"wt");
	fprintf(FO,"%d\n",SOM_V2_MAGIC);
	outM(CS,1,CP-1);
	dumpString();
	outM(M,STRBASE,STRBASE+freestr-1);
	fclose(FO);
}
*/
int main( int argc, char *argv[] ){
	string source, code, list, iasm, name;
	char *fname;

	if( argc < 2 ) {
		printf("usage : compile source\n");
		exit(-1);
	}
	strcpy(source,argv[1]);
	strcpy(name,source);
	fname = strtok(name,".");
	if( fname == NULL ) fname = name;
	strcat2(code,fname,".obj");
//	strcat2(list,fname,".lst");
//	strcat2(iasm,fname,".s2");

	readinfile(source);
//	prolog();
//	testlex(); exit(0);

	mylex2();
	asm2(); exit(0);
/*
	pass();
	showParseTree();
//	showGlobal();
	genall();
//  dumpSymTab();
	epilog();
	FO = stdout;
	showCode(3,CP-1);
	outObj(code);
*/
	return 0;
}

