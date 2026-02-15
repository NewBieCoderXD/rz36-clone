/* symtab.c   R1 compiler
	P. Chongstitvatana
	19 Aug 97
*/


#include "compile.h"

#define  eqs(a,b)  (strcmp(a,b) == 0)

//int scount = 0;
/*
==========  Interface =============

// search local and insert if not found, ret index or 0 (not found)
int installLocal(char *name, int *found)

// search global and insert if not found, ret index or 0 (not found)
int installGlobal(char *name, int type, int ref, int arg, int *found)

// search local and global if not found insert into local, ret index
int install(char *name)

// get attribute at index *
void getAt(int index, int *type, int *ref, int *arg)

// update ref
void setRef(int index, int ref)

void dumpSymTab()
====================================
*/

/* symbols start from index 1, reserve index 0 */
PRIVATE sym_entry symtab[TABLESIZE+LOCALSIZE];
int nsym = 0;
int lv = 0;

/* sequential search ret index to symtab[] if found, 0 if not */
PRIVATE int searchLocal(char *name){
	int i;
	for(i=TABLESIZE+1;i<=TABLESIZE+lv;i++) {
//		scount++;
		if( eqs(symtab[i].name, name) ) break;
	}
	return i > TABLESIZE+lv ? 0 : i;
}

/* insert local symbol at the end of table */
PRIVATE int enterLocal(char *name){
	int a;
	lv++;
//	if( lv >= LOCALSIZE ) seterror("local symbol table full");
	a = TABLESIZE + lv;
	strcpy(symtab[a].name, name);
	symtab[a].type = tyLOCAL;
	symtab[a].ref = lv;
	symtab[a].arg = 0;
	return a;
}

PRIVATE int searchGlobal(char *name){
	int i;
	for(i=1;i<=nsym;i++) {
//		scount++;
		if( eqs(symtab[i].name, name) ) break;
	}
	return i > nsym ? 0 : i;
}

/* insert global symbol at the end of table */
PRIVATE int enterGlobal(char *name, int type, int ref, int arg){
	int a;
	nsym++;
//	if( nsym >= TABLESIZE ) seterror("global symbol table full");
	a = nsym;
	strcpy(symtab[a].name, name);
	symtab[a].type = type;
	symtab[a].ref = ref;
	symtab[a].arg = arg;
	symtab[a].fs = 0;
	return a;
}

/* search local and insert if not found, ret index or 0 (not found)
   use when want to check duplicate */
int installLocal(char *name, int *found){
	int a;
	*found = 1;
	a = searchLocal(name);
	if( a != 0 ) return a;
	*found = 0;
	a = enterLocal(name);
	return a;
}

/* search global and insert if not found, ret index or 0 (not found)
   use when want to check duplicate */
int installGlobal(char *name, int type, int ref, int arg, int *found){
	int a;
	*found = 1;
	a = searchGlobal(name);
	if( a != 0 ) return a;
	*found = 0;
	a = enterGlobal(name,type,ref,arg);
	return a;
}

/* search local and global if not found insert into local, ret index */
int install(char *name){
	int a;
	a = searchLocal(name);
	if( a != 0 ) return a;
	a = searchGlobal(name);
	if( a != 0 ) return a;
	a = enterLocal(name);
	return a;
}

/* get attribute at index
void getAt(int index, int *type, int *ref, int *arg){
	*type = symtab[index].type;
	*ref = symtab[index].ref;
	*arg = symtab[index].arg;
}
*/

// access functions

int getType(int idx){ return symtab[idx].type; }
int getRef(int idx){ return symtab[idx].ref; }
char *getName(int idx){ return symtab[idx].name; }
int getArg(int idx){ return symtab[idx].arg; }
int getFs(int idx){ return symtab[idx].fs; }

void setType(int idx, int val){ symtab[idx].type = val; }
void setRef(int idx, int val){ symtab[idx].ref = val; }
void setArg(int idx, int val){ symtab[idx].arg = val; }
void setFs(int idx, int val){ symtab[idx].fs = val; }

// search global for name with type,ref
char *findName(int type, int ref){
	int i;
	for(i=1; i<=nsym; i++) {
		if( symtab[i].type == type && symtab[i].ref == ref)
			return getName(i);
	}
	return NULL;
}

int getMainRef(void){
	int ref;
	ref = searchGlobal("main");
	if( ref != 0 ) return getRef(ref);
	return 0;
}

/* ========= for debugging =========
void dumpSymTab()
{
  int i;
  sym_entry *s;
  printf("symbol table %d entries\n%s\n",nsym,
  	"      name type ref arg");
  for( i=1; i<=nsym; i++ ) {
    s = &symtab[i];
    printf("  %d %-8s %3d %3d %3d\n",
      i,s->name,s->type,s->ref,s->arg);
  }
}

void dumpSym2(){
	int i;
	sym_entry *s;
	for(i=1;i<=nsym;i++){
		s = &symtab[i];
		printf("%d name %s type %d ref %d arg %d\n",
		  i, s->name, s->type, s->ref, s->arg);
	}
	for(i=TABLESIZE;i<=TABLESIZE+lv; i++){
		s = &symtab[i];
		printf("%d name %s type %d ref %d arg %d\n",
		  i-TABLESIZE, s->name, s->type, s->ref, s->arg);
	}
}
*/

