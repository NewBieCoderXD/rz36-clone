/* lex.c   lexical analyzer   for R1
	15 Oct 97
	binary search reserved word  		2 Sept 97
		start project RZ				3 Apr 2001
	change to accept token()			15 Apr 2001
	add lineat[] and getsrc() to track source code line  20 December 2001
*/

#include "compile.h"

#define MAXFIN	10000	/* max input file size */
#define MAXLINE  1000	/* max no. of source line */

#define	cBLANK		0	// character type
#define	cLETTER     1
#define	cDIGIT      2
#define cQUOTE		3
#define cSPECIAL	4

//extern int CPLINE;
extern int token(void);			/* in token.c */

int tok;		     			/* current token type, string */
char tokstring[256];
int line = 1;					/* current position */
uchar CH;       				/* current char */

PRIVATE char inbuf[MAXFIN];		/* input file buffer */
PRIVATE char *cp, *ieof, *cp1;

// character type table, create by initCharType()
PRIVATE char chartype[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,4,3,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,4,4,
  4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,4,4,4,4,1,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4
};

PRIVATE char *lineat[MAXLINE];

void getC(void){
	if ( cp >= ieof ) {
		CH = EOF_CHAR; cp = ieof;
	}else
		CH  = *cp++;
}

PRIVATE void unget(void) {
	cp--; CH = *(cp-1);
}

PRIVATE void doline(void){   // store cp to the current line
	line++;
	lineat[line] = cp;
}

PRIVATE void skipblank(void)
{
	for(;;) {
		while( chartype[CH] == cBLANK ) {
			if ( CH == NL_CHAR) doline();
//			CPLINE = line; /* add */
			getC();
		}
		if( CH != '/' ) break;
		getC();
		if( CH != '/' ) { unget(); break; }
		while( CH != NL_CHAR && CH != EOF_CHAR) getC();
	}
}

PRIVATE void copyToken(char* a, char* b) {
	int len;
	len = b - a;
	if( len <= 0 ) {
		printf("lex error token length = 0\n");
		exit(-1);
	}
	if( len > 255 ) {
		printf("error token too long > 256 char\n");
		exit(-1);
	}
	strncpy(tokstring,a,len);
	tokstring[len] = 0;
}

/* ------------- public ------------------ */

int isDigit(uchar c){ return (chartype[c] == cDIGIT); }

int isLetterOrDigit(uchar c){
	return  (chartype[c] == cLETTER || chartype[c] == cDIGIT);
}

void accept(int v, int k){
	tok = v;
	cp = cp1 + k;
	getC();
}

//void lex(void){ mylex(); }

void mylex(void){
	skipblank();
	if( CH == EOF_CHAR ) {tok = tkEOF; return; }
	cp1 = cp - 1;
	if( token() ) return;

	/* recogniser reject, reset char pointer */
	cp = cp1;
	getC();
	switch( chartype[CH] ) {
	case cLETTER :
		while( isLetterOrDigit(CH) ) getC();
		copyToken(cp1,cp-1);
		tok = tkIDEN;
		break;
	case cDIGIT :
		while( isDigit(CH) ) getC();
		copyToken(cp1,cp - 1);
		tok = tkNUMBER;
		break;
	case cQUOTE :
		do {
			getC();
			if ( CH == NL_CHAR ) doline();
		}  while( CH != '"' && CH != EOF_CHAR);
		copyToken(cp1+1,cp-1);
		getC();
		tok = tkSTRING;
		break;
	default : tok = tkERROR;
	}
//    prtoken();
}

void readinfile(char *fname) {
	FILE *fp;
	int n;
	fp = fopen(fname,"rt");
	if( fp == NULL ) {
		printf("cannot open : %s\n",fname);
		exit(-1);
	}
	n = fread(inbuf,1,MAXFIN,fp);
	fclose(fp);
	cp = inbuf;
	ieof = cp + n;
	getC();				// start lex
}

/*
PRIVATE char srcbuf[500];

char *getsrc(int line){	// return source line
	char *c;
	int n;
	c = lineat[line];
	if( c == NULL ) return NULL;
	n = strcspn(c,"\n");
	strncpy(srcbuf,c,n);
	srcbuf[n] = 0;
	return srcbuf;
}

void initCharType()
{
	int i;
	for(i=0; i<256; i++) chartype[i] = cBLANK;
	for(i=33; i<128; i++ ) chartype[i] = cSPECIAL;
	for(i='0'; i<='9'; i++ ) chartype[i] = cDIGIT;
	for(i='A'; i<='Z'; i++ ) chartype[i] = cLETTER;
	for(i='a'; i<='z'; i++ ) chartype[i] = cLETTER;
	chartype['_'] = cLETTER;
	chartype['"'] = cQUOTE;
	chartype[EOF_CHAR] = cSPECIAL;
}

void printchartype(){
	int i;
	for(i=0; i<256; i++){
		printf("%d,",chartype[i]);
		if(i%30 == 29) printf("\n");
	}
}
*/

