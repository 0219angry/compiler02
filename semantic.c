#include "token-list.h"

/* scan.c */
extern int current_line;


char procname_attr[MAXSTRSIZE];
ID *tempID;
int isloot = 0; /* 0:global 1:local */
int ispara = 0; /* 0:parameter 1:variable */

/* cross reference ID List loot */
ID *globalidloot;
ID *localidloot;
ID *cridloot;

void init_semantic(){
  
}

void clear_temp(){
}


/* 変数名を追加(変数宣言部で使用) 
 * 

*/
void add_define_without_type(){
  ID *newdef = malloc_ID();
  char *name = malloc(sizeof(char) * MAXSTRSIZE);
  strcpy(name, string_attr);
  newdef->name = name;
  char *procname = malloc(sizeof(char) * MAXSTRSIZE);
  strcpy(procname, procname_attr);
  newdef->procname = procname;
  newdef->itp = NULL;
  newdef->ispara = ispara;
  newdef->deflinenum = current_line;
  newdef->nextp = NULL;
  if(tempID == NULL){
    tempID = newdef;
  }else{
    ID *p;
    p = tempID;
    while(p->nextp != NULL){
      p = p->nextp;
    }
    p->nextp = newdef;
  }
}

/* tempIDのリストの要素すべての型にtyを追加 
 * @param[in] ty 追加するTYPE
 */
void add_type(TYPE *ty){
  ID *p = tempID;
  p->itp = ty;
  while(p->nextp != NULL){
    p = p->nextp;
    p->itp = ty;
  }
}

/*
 TYPEを作成する
 @param[in] ttype 型
 @param[in] arraysize (arrayのとき)配列長さ*/
TYPE * create_type(int ttype, int arraysize){
  TYPE *typeptr;
  typeptr = malloc_type();
  switch(ttype){
    case TPINT:
    case TPBOOL:
    case TPCHAR:
      typeptr->ttype = ttype;
      break;
    case TPARRAYINT:
    case TPARRAYBOOL:
    case TPARRAYCHAR:
      typeptr->ttype = TPARRAY;
      TYPE *typeptrarray;
      typeptrarray = malloc_type();
      typeptr->etp = typeptrarray;
      typeptrarray->ttype = ttype;
      typeptr->arraysize = arraysize;
      break;
    case TPPROC:
      typeptr->ttype = TPPROC;
      // PROCが来たら何とかする
      break;
    default:
      error("unknown type.");
  }
}

/*
 *
*/
TYPE *add_formal_type(TYPE *loot, int ttype){
  TYPE *p = loot;
  while(p->paratp != NULL){
    p = p->paratp;
  }
  TYPE *newformaltype = malloc_TYPE();
  newformaltype->ttype = ttype;
  p->paratp = newformaltype;
  return p;
}


/* regist_define : 変数名をクロスリファレンサに登録
   isloot:0 globalidlootの末尾に追加 isloot:1 localidlootの末尾に追加 */
void resist_define(){
  ID *p;
  if(isloot == 0){
    p = globalidloot;
  }else if(isloot == 0){
    p = localidloot;
  }
}

ID * add_reference(char *name, int num){
  ID *idp;
  if(isloot == 0){
    idp = search_ID(globalidloot, name);
  }else{
    idp = search_ID(localidloot, name);
  }
  if(idp = NULL){
    error("undifined name is referenced.");
    return(NULL);
  }
  LINE *newref = malloc(sizeof(LINE));
  newref->reflinenum = num;
  newref->nextlinep = NULL;
  LINE *linep = idp->irefp;
  if(linep == NULL){
    idp->irefp = newref;
  }else{
    while(linep->nextlinep != NULL){
      linep = linep->nextlinep;
    }
    linep->nextlinep = newref;
  }
  return idp;
}

ID * search_ID(ID *top, char *name){
  ID *p;
  p = top;
  while(p->nextp != NULL){
    if(strcmp(p->name,name) == 0){
      return p;
    }
    p = p->nextp;
  }
  return NULL;
}

/*
 * IDを一つつくって初期化
 * param [out] 作ったIDへのポインタ
*/
ID * malloc_ID(){
  ID *id;
  id = malloc(sizeof(ID));
  id->name = NULL;
  id->procname = NULL;
  id->itp = NULL;
  id->ispara = 0;
  id->deflinenum = 0;
  id->irefp = NULL;
  id->nextp = NULL;
  return id;
}

/*
 * TYPEを一つつくって初期化
 * param [out] 作ったTTYPEへのポインタ
*/
TYPE * malloc_TYPE(){
  TYPE *type;
  type = malloc(sizeof(TYPE));
  type->ttype = 0;
  type->arraysize = 0;
  type->etp = NULL;
  type->paratp = NULL;
  return type;
}






// ID * add_define(){
//   ID *newdef = malloc(sizeof(ID));
//   newdef->name = tempID->name;
//   newdef->procname = tempID->procname;
//   newdef->itp = tempID->itp;
//   newdef->ispara = tempID->ispara;
//   newdef->deflinenum = tempID->deflinenum;

//   ID *p;
//   if(newdef->procname == NULL){
//     p = globalidloot;
//   }else{
//     p = localidloot;
//   }
//   while(p->nextp != NULL){
//     p = p->nextp;
//   }
//   p->nextp = newdef;

//   clear_temp();
//   return newdef;
// }