/* icode.c
	P. Chongstitvatana

	project Som						13 Sept 2003
	code generator som-c			2 Jan 2004
	new tailcall					25 Dec 2004 Merry Christmas!
	som-v2-c listing to teach compiler, use with Z-assembler
								    15 July 2011 (Ar-sa-ra-hat day)
*/

#include "compile.h"
#include "icname.h"

#define		PRIVATE		static

extern FILE *FO;
extern int currentf;	// gencode.c  current func idx

int CS[MAXCS];
int CP = 3;				// current code pointer

PRIVATE void decode(int ip, int *code, int *data){
	*code = CS[ip] & 255;
	*data = CS[ip] >> 8;
}

PRIVATE void putcode(int ip, int code, int data){
	CS[ip] = (data << 8) | (code & 255);
}

PRIVATE void pl(char *s1){
	fprintf(FO,"    %s",s1);
}
PRIVATE void pl2(char *s1, int a){
	fprintf(FO,"    %s.%d",s1,a);
}
PRIVATE void pJ(char *s1, int a){
	fprintf(FO,"    %s.L%d",s1,a);
}
PRIVATE void plnm(char *s1, char *s2){
	fprintf(FO,"    %s.%s",s1,s2);
}
PRIVATE void plabel(char *s1){
	fprintf(FO,":%s\n",s1);
}

PRIVATE void prCode(int ip){
	int op, a;
	char *nm, buf[80];
	decode(ip,&op,&a);
	switch(op){
	case icAdd: pl("add"); break;
	case icSub: pl("sub"); break;
	case icMul: pl("mul"); break;
	case icDiv: pl("div"); break;
	case icBand: pl("and"); break;
	case icBor: pl("or"); break;
	case icBxor: pl("xor"); break;
	case icNot: pl("not"); break;
	case icEq: pl("eq"); break;
	case icNe: pl("ne"); break;
	case icLt: pl("lt"); break;
	case icLe: pl("le"); break;
	case icGe: pl("ge"); break;
	case icGt: pl("gt"); break;
//	case icShl: pl("shl"); break;
//	case icShr: pl("shr"); break;
	case icMod: pl("mod"); break;
	case icLdx: pl("ldx"); break;
	case icStx: pl("stx"); break;
	case icRet: pl2("ret",a); break;
	case icArray: pl("array"); break;
	case icEnd: pl("sys.13"); break;
	case icGet: pl2("get",a); break;
	case icPut: pl2("put",a); break;
//	case icLd: plnm("ld",findName(tySCALAR,a)); break;
//	case icSt: plnm("st",findName(tySCALAR,a)); break;
	case icLd: pl2("ld",a); break;
	case icSt: pl2("st",a); break;
	case icJmp: pJ("jmp",ip+a); break;
	case icJt: pJ("jt",ip+a); break;
	case icJf: pJ("jf",ip+a); break;
	case icLit: pl2("lit",a); break;
//	case icCalli: plnm("call",getName(a)); break;
	case icCall: plnm("call",findName(tyFUNCTION,a)); break;
	case icInc: pl2("inc",a); break;
	case icDec: pl2("dec",a); break;
	case icSys: pl2("sys",a); break;
	case icFun:
		plabel(findName(tyFUNCTION,ip));
		pl2("fun",a);
		break;
	case icRef: pl2("ref",a); break;
	default: pl("undef");
	}
}

void outM(int *mem, int start, int end){
	int i, op, data;
	fprintf(FO,"%d %d\n",start,end);
	for(i=start; i<=end; i++){
		fprintf(FO,"%d ",mem[i]);
		if( (i % 8 ) == 0 ) fprintf(FO,"\n");
	}
	fprintf(FO,"\n");
}
/*
int readM(int *mem, FILE *fi){
	int i, b, e, c;
	fscanf(fi,"%d%d",&b,&e);
	for(i=b; i<=e; i++){
		fscanf(fi,"%d",&c);
		mem[i] = c;
	}
	return e;
}
*/

PRIVATE int isJmp(int op){
	return op == icJmp | op == icJt | op == icJf;
}

PRIVATE char markLabel[MAXCS];		// mark label

void showCode(int start, int end){
	int i, op, a;
	for(i=start; i<=end; i++){
		decode(i,&op,&a);
		if( isJmp(op) ) markLabel[i+a] = 1;
	}
	for(i=start; i<=end; i++){
		if( markLabel[i] ) fprintf(FO,":L%d\n",i);
		prCode(i);
		fprintf(FO,"\n");
	}
}

// out code with argument
void outa( int code, int arg){
//	printf("@ %d outa %d %d\n",CP,code,arg);
	CS[CP] = (arg << 8 ) | (code & 255);
	CP++;
}

// out singleton, arg = 0
void outs( int code){
//	printf("@ %d outs %d\n",CP,code);
	CS[CP] = code;
	CP++;
}

// change arg, preserve code
void patch( int ads, int v ){
	CS[ads] = (v << 8) | (CS[ads] & 255);
}

// the end of body is at CP
PRIVATE void reName(int fn){
	int i, c, d, local, ref;
	local = getFs(fn);
	ref = getRef(fn);
	for( i=ref+1; i<CP; i++ ) {
		decode(i,&c, &d);
		switch( c ) {
		case icRef :
		case icGet :
		case icPut :
		case icInc :
		case icDec : patch(i, local-d+1); break;
		}
	}
}
/*
PRIVATE void tailCall(int ref){
	int i, a, c1, d1;
	for(i=ref+2; i<CP; i++){
	    decode(i,&c1,&d1);
		if( c1 == icRet ){
			decode(i-1,&c1,&d1);
			if( c1 == icCall && d1 == ref )
				putcode(i-1,icCallt,d1);
		}
	}
}
*/
/*
// ref now is at "ret"
PRIVATE void tailCall2(int ref){
	int i, ref2, c, d, c1, d1, arg;
	ref2 = getRef(currentf);
	decode(ref,&c,&d);			// ret
	decode(ref-1,&c1,&d1);		// call
	if( c1 == icCall && d1 == ref2 ){
		printf("tail call\n");
		arg = getArg(currentf);
		CP = ref - 1;			// replace "call"
		for(i=arg; i>=1; i--)
			outa(icPut,i);
		outa(icJmp,ref2-CP+1);
		outa(icRet,d);	// necessary !
		// correct jump to ref
		for(i=ref2; i<ref; i++){
			decode(i,&c,&d);
			if( isJmp(c) && ((d+i) == ref))
				patch(i,CP-1-i);
		}
	}
}

// do a small improvement: short cut jump to ret, jmp to jmp
PRIVATE void improv2(int ref){
	int i,c1,d1,c2,d2;
	// replace jmp to ret, jmp to jmp
	for( i=ref+1; i<CP-1; i++ ){
		decode(i,&c1,&d1);
		if( isJmp(c1) ){
		    decode(i+d1,&c2,&d2);
			if( c1 == icJmp && c2 == icRet )
				putcode(i,icRet,d2);
			else if( c2==icJmp )
				patch(i,d1+d2);
		}
	}
}
*/
/* if body of fun is empty clear header
	header format is:  fun/idx, arity, lv, size
	new header is : Fun/arity:lv in one int

void clean(int fn) {
  int ref, a;
  ref = getRef(fn);
  if ( (CP-1) == ref ){			// empty body
	setRef(fn,0);				// indicate undef
	CP--;
	ref = 0;
  }else {
	patchbreak();
	a = getLv(fn) + 1;
	outa(icRet,a);
	// update fun header to any change of lv from for
//	a = CS[ref] >> 8;		// arity:lv
//	a = (getLv(fn) << 8) + (a & 255);
	a = getLv(fn) - getArg(fn) + 1;
	putcode(ref,icFun,a);
  }
  if( ref != 0 ){
    reName(ref);
    improv2(ref);
	improv2(ref);
    tailCall(ref);
  }
}
*/
// only when function body is not empty
void clean(int fn) {
	int ref, a;
	ref = getRef(fn);
	patchbreak();
	a = getFs(fn) + 1;		// ***
	outa(icRet,a);
//	tailCall2(CP-1);	// at "ret"
	// update fun header to any change of lv
	a = getFs(fn) - getArg(fn) + 1;		// ***
	putcode(ref,icFun,a);
    reName(fn);
//	improv2(ref);
//	improv2(ref);
}

// change Calli to Call from CS[a]..CS[b]
void patchCalli(int a, int b){
	int i, c, d;
	for(i=a; i<=b; i++){
		decode(i,&c,&d);
		if( c == icCalli )
			putcode(i,icCall,getRef(d));
	}
}
