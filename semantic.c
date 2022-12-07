#include "token-list.h"


ID *tempID;
int isloot = 0; /* 0:global 1:local */

/* cross reference ID List loot */
ID *globalidloot;
ID *localidloot;

void init_semantic(){
  tempID = malloc(sizeof(ID));
  
}

void clear_temp(){
  tempID->name = NULL;
  tempID->procname = NULL;
  tempID->itp = NULL;
  tempID->ispara = 0;
  tempID->deflinenum = 0;
  tempID->irefp = NULL;
  tempID->nextp = NULL;
}

ID * add_define(){
  ID *newdef = malloc(sizeof(ID));
  newdef->name = tempID->name;
  newdef->procname = tempID->procname;
  newdef->itp = tempID->itp;
  newdef->ispara = tempID->ispara;
  newdef->deflinenum = tempID->deflinenum;

  ID *p;
  if(newdef->procname == NULL){
    p = globalidloot;
  }else{
    p = localidloot;
  }
  while(p->nextp != NULL){
    p = p->nextp;
  }
  p->nextp = newdef;

  clear_temp();
  return newdef;
}

ID * add_reference(char *name, int num){
  ID *idp;
  if(isloot == 0){
    idp = search_ID(globalidloot, name);
  }else{
    idp = search_ID(localidloot, name);
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