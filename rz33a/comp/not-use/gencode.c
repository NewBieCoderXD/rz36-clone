/* gencode.c
   code generator som-c		2 Jan 2004
	improve clean, not gen when empty body  14 Mar 2004
*/

#include "compile.h"
#include "icname.h"

#define		PRIVATE		static

#define head(e)		car(e)
#define tail(e) 	cdr(e)
#define arg1(e)		car(e)
#define arg2(e)		item2(e)
#define arg3(e)		item3(e)

int M[MAXMEM];
//extern int M[];

extern int CP;				/* current code pointer */

int currentf;		/* current function idx */
int fp;

PRIVATE int atomeq(int e, int type, int value){
	return (car(e) == type) && (cdr(e) == value);
}

// e = (lv (+ lv 1))
PRIVATE int isinc(int e){
	int a, ref, op;
	a = car(e);
	if( car(a) != LNAME ) return 0;
	ref = cdr(a);
	e = item2(e);		// e = (+ lv 1)
	op = car(e);
	if( !(atomeq(op,OPER,tkPLUS) || atomeq(op,OPER,tkMINUS)) )
		return 0;
	a = item2(e);
	if( ! atomeq(a,LNAME,ref) ) return 0;
	if( ! atomeq(item3(e),NUM,1) ) return 0;
	return 1;
}

PRIVATE void genbop(int op, int l){
	genex(car(l));
	genex(item2(l));
	outs(op);
}

PRIVATE void genuop(int op, int l){
	genex(car(l));
	outs(op);
}

// gen vec address:  (name ex) => ld/get, ex
PRIVATE void genvec(int e){
	int nm, type, ref;
	nm = car(e);
	type = car(nm);
	ref = cdr(nm);
	switch(type){
	case GNAME: outa(icLit,getRef(ref)); break;
	case LNAME: outa(icGet,ref); break;
	}
	genex(item2(e));
}

// return patch-address  e = (e1 e)
PRIVATE int genif(int e){
	int ads;
	genex(car(e));
	outa(icJf,0);
	ads = CP-1;
	genex(item2(e));
	return ads;
}

// if there is any break, patch jump to CP
void patchbreak(void){
	int ads;
	if( ytos() == MARK ){	// TO without break
		ypop();
		return;
	}
	while( ytos() == BMARK ){
		ypop();			// throw away BMARK
		ads = ypop();
		patch(ads,CP-ads);
	}
	if( ytos() != MARK )
		seterror("patch-break no matching MARK");
	ypop();				// throw away MARK
}

// (ex..ex)
PRIVATE void genlist(int e){
	while( e != NIL ){
		genex(car(e));
		e = cdr(e);
	}
}

PRIVATE void genatom2(int x){
	int type, ref;

	type = car(x);
	ref = cdr(x);
	switch(type){
	case STRING: outa(icLit,ref+STRBASE); break;
	case NUM: outa(icLit,ref); break;
	case GNAME: outa(icLd,getRef(ref)); break;
	case LNAME: outa(icGet,ref); break;
	case OPER:
		switch(ref){
		case tkBREAK:
			ypush(CP);
			ypush(BMARK);
			outa(icJmp,0);
			break;
		}
		break;
	}
}
/*  short circuit logical and
x && y

if x then y else 0
get.x
jf.+2
get.y
jmp.+1
lit.0
*/
PRIVATE void genland(int e){
	int ads;
	genex(car(e));
	ads = CP;
	outa(icJf,0);
	genex(item2(e));
	outa(icJmp,2);
	patch(ads,CP-ads);
	outa(icLit,0);
}

/*  short circuit logical or
x || y

if x then 1 else y
get.x
jf.+2
lit.1
jmp.+1
get.y
*/
PRIVATE void genlor(int e){
	int ads;
	genex(car(e));
	outa(icJf,3);
	outa(icLit,1);
	ads = CP;
	outa(icJmp,0);
	genex(item2(e));
	patch(ads,CP-ads);
}

// deref x is (* v)
PRIVATE int isDeref(int x){
	return atomeq(car(x),OPER,tkDEREF);
}

void genex(int x){
	int a, e, e1, ads, ads2;
	int idx, type, ref;

	if( x == NIL ) return;
	if(isatom(x)){ genatom2(x); return; }

	a = car(x);			// (op ex .. )
	e = cdr(x);			// e = (ex .. )
	if(car(a) != OPER)
		seterror("genex: expect operator");
	switch(cdr(a)){
	case tkPLUS: genbop(icAdd,e); break;
	case tkMINUS: genbop(icSub,e); break;
	case tkSTAR: genbop(icMul,e); break;
	case tkSLASH: genbop(icDiv,e); break;
//	case tkAND: genbop(icBand,e); break;
//	case tkBAR: genbop(icBor,e); break;
//	case tkCARET: genbop(icBxor,e); break;
	case tkANDAND: genland(e); break;
	case tkOROR: genlor(e); break;
	case tkEQEQ: genbop(icEq,e); break;
	case tkNE: genbop(icNe,e); break;
	case tkLT: genbop(icLt,e); break;
	case tkLE: genbop(icLe,e); break;
	case tkGE: genbop(icGe,e); break;
	case tkGT: genbop(icGt,e); break;
//	case tkMOD: genbop(icMod,e); break;
//	case tkGTGT: genbop(icShr,e); break;
//	case tkLTLT: genbop(icShl,e); break;
	case tkUNOT: genuop(icNot,e); break;
//	case tkARRAY: genuop(icArray,e); break;
//	case tkSYSCALL: 			// e = (num ex0 .. ex0)
//		genlist(cdr(e));
//		outa(icSys,cdr(car(e)));
//		break;
	case tkDO:					// (block ex .. ex)
		genlist(e);				// e = (ex..ex)
		break;
	case tkCALL:				// (call name ex ... ex)
		idx = cdr(car(e));		// e = (name ex .. ex)
		genlist(cdr(e));
		outa(icCalli,idx);		// instantiate ref later
		break;
	case tkFUN:				// (fun name ex)
		idx = cdr(car(e));	// e = (name ex)
		currentf = idx;
		e = item2(e);
		if( e != NIL ){
			setRef(idx,CP);
			outa(icFun,0);	// clean will update it
			ypush(MARK);
			genex(e);
			clean(idx);
		}
		break;
	case tkDEREF:			// RHS *
		genex(car(e));
		outa(icLit,0);
		outs(icLdx);
		break;
	case tkADS:				// RHS &
		e1 = car(e);
		if(isatom(e1)){
			type = car(e1);
			ref = cdr(e1);
			switch(type){
			case GNAME: outa(icLit,getRef(ref)); break;
			case LNAME: outa(icRef,ref); break;
			}
		}else{				// (vec name ex)
			genvec(cdr(e1));
			outs(icAdd);
		}
		break;
	case tkVEC:
		genvec(e);			// vec
		outs(icLdx);
		break;
	case tkEQ:				// (= var/vec ex)
		a = car(e);			// e = (var/vec ex)
		e1 = item2(e);		// ex
		if(isinc(e)){		// e = (lv (+ lv 1))
			ref = cdr(a); 	// lvar ref
			if(atomeq(car(e1),OPER,tkPLUS))  // e1 = (+ lv 1)
				outa(icInc,ref);
			else
				outa(icDec,ref);
		}else if(isatom(a)){	// var
			genex(e1);			// RHS
			type = car(a);
			ref = cdr(a);
			switch(type){
			case GNAME: outa(icSt,getRef(ref)); break;
			case LNAME: outa(icPut,ref); break;
			}
		}else if(isDeref(a)){	// a is (* v)
			genex(item2(a));
			outa(icLit,0);
			genex(e1);			// RHS
			outs(icStx);
		}else{					// vec LHS
			genvec(cdr(a));
			genex(e1);			// RHS
			outs(icStx);
		}
		break;
	case tkIF:				// (if e1 e)
		ads = genif(e);		// e = (e1 e)
		patch(ads,CP-ads);
		break;
	case tkELSE: 			// (ifelse e1 et ef)
		ads = genif(e);		// e = (e1 et ef)
		outa(icJmp,0);
		patch(ads,CP-ads);
		ads = CP-1;
		genex(item3(e));
		patch(ads,CP-ads);
		break;
	case tkWHILE: 			// (while e1 e)
		ypush(MARK);
		ads = CP;			// e = (e1 e)
		outa(icJmp,0);		// branch to cond
		genex(item2(e));	// body of while
		ads2 = CP;
		genex(car(e));		// cond
		outa(icJt,ads+1-CP);// jump to body
		patch(ads,ads2-ads);
		patchbreak();
		break;
	case tkRETURN:
		genex(car(e));
		outa(icRet,getFs(currentf)+1);
		break;
	case tkPRINT:
		while( e != NIL ){
			e1 = head(e);
			genex(e1);
			if( head(e1) == STRING ) // type
				outa(icSys,6);
			else
				outa(icSys,1);
			e = tail(e);
		}
		break;
	default:
		seterror("unknown op\n");
	}
}
