#include "token-list.h"


char *typestr[NUMOFTYPE+1] = {
  "",
  "integer", "char", "boolean", "array", "integer", "char", "boolean", "procedure"
};




/* scan.c */
extern int current_line;

/* parse.c */
extern int arraysize;
extern int is_variable_declaration; /* 0:out declaration 1:in declaration */
extern int is_in_procedure; /* 0:out procedure 1:in procedure 2:procedure name*/
extern int is_call_statement; /* 0:not call 1:call */
extern int is_formal_parameter; /* 0:no 1:yes */

char *procname_attr = NULL;
ID *tempID = NULL;
int isloot = 0; /* 0:global 1:local */
int ispara = 0; /* 0:parameter 1:variable */

/* cross reference ID List loot */
ID *globalidloot = NULL;
ID *localidloot = NULL;
ID *procid = NULL;
PROC *cridloot = NULL;



/* 変数名を追加(変数宣言部で使用) 
 * 

*/
void add_define_without_type(){
  ID *newdef = malloc_ID();
  char *name;
  if((name = malloc(sizeof(char) * MAXSTRSIZE)) == NULL){
    error("memory error");
    exit(EXIT_FAILURE);
  }
  printf("%s",name);
  strcpy(name, string_attr);
  newdef->name = name;
  newdef->procname = procname_attr;
  newdef->itp = NULL;
  newdef->ispara = is_formal_parameter;
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
  if(is_in_procedure == 2){
    char *procname;
    if((procname = malloc(sizeof(char) * MAXSTRSIZE)) == NULL){
      error("memory error");
      exit(EXIT_FAILURE);
    }
   
    strcpy(procname, string_attr);
    procname_attr = procname;
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
TYPE * create_type(int ttype){
  TYPE *typeptr;
  typeptr = malloc_TYPE();
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
      typeptrarray = malloc_TYPE();
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
  arraysize = 0;
  return typeptr;
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
   is_in_procedure:0,2 globalidlootの末尾に追加 is_in_procedure:1 localidlootの末尾に追加 */
int regist_define(){
  ID *p;
  if(is_in_procedure == 1){
    if(localidloot == NULL){
      localidloot = tempID;
      tempID = NULL;
      return 0;
    }else{
      p = localidloot;
    }
  }else{
    if(globalidloot == NULL){
      globalidloot = tempID;
      tempID = NULL;
      return 0;
    }else{
      p = globalidloot;
    }
  }

  while(p->nextp != NULL){
    p = p->nextp;
  }
  p->nextp = tempID;
  tempID = NULL;
  return 0;
}

ID * add_reference(char *name, int num){
  ID *idp;

  
  if(is_in_procedure == 1){
    idp = search_ID(localidloot, name);
    if(idp == NULL){
      idp = search_ID(globalidloot, name);
    }
  }else{
    idp = search_ID(globalidloot, name);
    
  }
  if(idp == NULL){
    error("undifined name is referenced.");
    return(NULL);
  }
  LINE *newref = malloc_LINE();
  newref->reflinenum = num;
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

int regist_proc_global(){
  PROC *proc;
  proc = malloc_PROC();
  proc->iidp = globalidloot;
  PROC *p;
  p = cridloot;
  if(p == NULL){
    cridloot = proc;
    return 0;
  }
  while(p->nextp != NULL){
    p = p->nextp;
  }
  p->nextp = proc;
  return 0;
}

int regist_proc_local(){
  PROC *proc;
  proc = malloc_PROC();
  proc->procname = procname_attr;
  proc->iidp = localidloot;
  PROC *p;
  p = cridloot;
  if(p == NULL){
    cridloot = proc;
    localidloot = NULL;
    return 0;
  }
  while(p->nextp != NULL){
    p = p->nextp;
  }
  p->nextp = proc;
  localidloot = NULL;
  return 0;
}



ID * search_ID(ID *top, char *name){
  ID *p;
  p = top;
  if(top == NULL){
    return NULL;
  }
  while(p != NULL){
    if(strcmp(p->name,name) == 0){
      return p;
    }
    p = p->nextp;
  }
  return NULL;
}

/*
 * IDを一つつくって初期化
 * @param [out] 作ったIDへのポインタ
*/
ID * malloc_ID(){
  ID *id;
  id = malloc(sizeof(ID));
  if(id == NULL){
    error("memory error");
    exit(EXIT_FAILURE);
  }
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
  if(type == NULL){
    error("memory error");
    exit(EXIT_FAILURE);
  }
  type->ttype = 0;
  type->arraysize = 0;
  type->etp = NULL;
  type->paratp = NULL;
  return type;
}

/*
 * PROCを一つつくって初期化
 * param [out] 作ったPROCへのポインタ
*/

PROC * malloc_PROC(){
  PROC *proc;
  proc = malloc(sizeof(PROC));
  if(proc == NULL){
    error("memory error");
    exit(EXIT_FAILURE);
  }
  proc->procname = NULL;
  proc->iidp = NULL;
  proc->nextp = NULL;
  return proc;
}

LINE * malloc_LINE(){
  LINE *line;
  line = malloc(sizeof(LINE));
  if(line == NULL){
    error("memory error");
    exit(EXIT_FAILURE);
  }
  line->reflinenum = 0;
  line->nextlinep = NULL;
  return line;
}

int check_overloading(char * name){
  ID * id;
  char mes[MAXSTRSIZE];
  if(is_in_procedure == 2 || is_in_procedure == 0){
    if((id = search_ID(globalidloot,name)) != NULL){
      sprintf(mes,"Variable %s is overloading (last difined in Line %d)",name,id->deflinenum);
      error(mes);
      return ERROR;
    }
  }else if(is_in_procedure == 1){
      if((id = search_ID(localidloot,name)) != NULL){
      sprintf(mes,"Variable %s:%s is overloading (last difined in Line %d)",name,id->procname,id->deflinenum);
      error(mes);
      return ERROR;
    }
  }
  return NORMAL;
}

 
void print_cridloot(){
  PROC *procp;
  ID *idp;
  TYPE *typep;
  char output[MAXSTRSIZE];
  int NumOfChar;
  procp = cridloot;

  printf("===============================================================================================\n");
  printf("Name                          Type                          Def. para Ref.                          \n");
  
  while(procp != NULL){
    idp = procp->iidp;

    procp = procp->nextp;
    int i=0;

    while(idp != NULL){
      i++;
      // Name print
      if(idp->procname == NULL){
        printf("%-30.30s",idp->name);
      }else{
        snprintf(output,MAXSTRSIZE,"%s:%s",idp->name,idp->procname);
        printf("%-30.30s",output);
      }
 
      // Type print
      typep = idp->itp;
      NumOfChar = 0;
      if(typep->ttype == TPPROC){
        NumOfChar += printf("%s",get_type_name(typep->ttype));
        while(typep->paratp != NULL){
          typep = typep->paratp;
          NumOfChar += printf("(%s",get_type_name(typep->ttype));
          if(typep->paratp != NULL){
            NumOfChar += printf(",");
          }
          NumOfChar += printf(")");
        }
      }else if(typep->ttype == TPARRAY){
        NumOfChar += printf("%s [%d] of ",get_type_name(typep->ttype),typep->arraysize);
        typep = typep->etp;
        NumOfChar += printf("%s",get_type_name(typep->ttype));
      }else{
        NumOfChar += printf("%s",get_type_name(typep->ttype));
      }
      for(int i=NumOfChar;i<30;i++){
        printf(" ");
      }
     
      // Def. print
      
      printf("%-5d",idp->deflinenum);

      // ispara print

      if(idp->ispara == 0){
        printf("n    ");
      }else{
        printf("y    ");
      }

      // Ref. print

      NumOfChar = 0;
      LINE *linep;
      linep = idp->irefp;
      
      while(linep != NULL){
        NumOfChar += printf("%d",linep->reflinenum);
        linep = linep->nextlinep;
        if(linep != NULL){
          NumOfChar += printf(", ");
        }
      }

      for(int i=NumOfChar;i<30;i++){
        printf(" ");
      }
 
      // \n print
      printf("\n");
      idp = idp->nextp;
    }
  }
  printf("===============================================================================================\n");
}



char * get_type_name(int ttype){
  return typestr[ttype-9];
}

int print_ID_for_debug(ID * loot){
  ID *p = loot;
  printf("\n*****DEBUG*****\n");
  if(p == NULL){
    printf("***************\n");
    return 0;
  }
  while(p->nextp != NULL){
    printf("name:%s proc:%s next:%p\n",p->name,p->procname,p->nextp);
    p = p->nextp;
  }
  printf("***************\n");
  return 0;
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