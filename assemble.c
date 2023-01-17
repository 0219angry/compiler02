#include "token-list.h"



int asm_start(char *programname){
  fprintf(caslfilep,"$$%s\tSTART\t\n",programname);
  fprintf(caslfilep,"\tLAD\tgr0, 0\n");
  fprintf(caslfilep,"\tCALL\t$block%%%s\n",programname);
  fprintf(caslfilep,"\tCALL\tFLUSH\n");
  fprintf(caslfilep,"\tSVC\t0\n");
  return 0;
}

int asm_return_st(){
  fprintf(caslfilep,"\tRET\t\n");
  return 0;
}

int asm_proc_val(ID * localidloot){
  ID * id = localidloot;
  while(id != NULL){
    switch((id->itp)->ttype){
      case TPINT:
      case TPCHAR:
      case TPBOOL:
        fprintf(caslfilep,"$%s%%%s\tDC\t0\n",id->name,id->procname);
        break;
      case TPARRAY:
        fprintf(caslfilep,"$%s%%%s\tDS\t%d\n",id->name,id->procname,(id->itp)->arraysize);
        break;
      default:
        return(error("forbidden type."));
    }
    id = id->nextp;
  }
  return 0;
}

int asm_proc_start(ID * localidloot,char * procname){
  ID * idp = localidloot;
  fprintf(caslfilep,"$%s\tDS\t0\n",procname);
    fprintf(caslfilep,"\tPOP\tgr2\n");
    ID *bp = NULL;
    while(bp != localidloot){
      while(idp->nextp != bp){
        idp = idp->nextp;
      }
      if(idp->ispara == 1){
        fprintf(caslfilep,"\tPOP\tgr1\n");
        fprintf(caslfilep,"\tST\tgr1, $%s%%%s\n",idp->name,idp->procname);
      }
      bp = idp;
      idp = localidloot;
    }
    fprintf(caslfilep,"\tPUSH\tgr2\n");
  return 0;
}

int asm_ref_lval(ID * refed){
  if(refed->ispara == 1){
    fprintf(caslfilep,"\tLD\tgr1, $%s%%%s\n",refed->name,refed->procname);
  }else if(refed->procname != NULL){
    fprintf(caslfilep,"\tLAD\tgr1, $%s%%%s\n",refed->name,refed->procname);
  }else{
    fprintf(caslfilep,"\tLD\tgr1, $%s\n",refed->name);
  }
  if((refed->itp)->ttype ==TPARRAY){
    //array型のときの処理
    fprintf(caslfilep,"\tPOP\tgr2\n"); // gr2 is index number.
    fprintf(caslfilep,"\tLAD\tgr3, %d\n",(refed->itp)->arraysize); // gr3 is max index number.
    fprintf(caslfilep,"\tCPA\tgr2,gr3\n");
    fprintf(caslfilep,"\tJPL\tEROV\n");
    fprintf(caslfilep,"\tADDA\tgr1, gr2\n");
    fprintf(caslfilep,"\tJOV\tOVF\n");
  }
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_ref_rval(ID * refed){
  asm_ref_lval(refed);
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tLD\tgr1, 0, gr1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_expression(){
  return 0;
}

int asm_minus_sign(){
  fprintf(caslfilep,"\tLAD\tgr1,-1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  asm_MULA();
  return 0;
}

int asm_ADDA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tADDA\tgr1, gr2\n");
  fprintf(caslfilep,"\tJOV\tOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_SUBA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tSUBA\tgr1, gr2\n");
  fprintf(caslfilep,"\tJOV\tOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_OR(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tOR\tgr1, gr2\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_MULA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tMULA\tgr1, gr2\n");
  fprintf(caslfilep,"\tJOV\tOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_DIVA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tADDA\tgr1, gr2\n");
  fprintf(caslfilep,"\tJOV\tOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_AND(){
  return 0;
}

int asm_read(ID * refed){
  asm_ref_lval(refed);
  if((refed->itp)->ttype == TPINT){
    fprintf(caslfilep,"\tCALL\tREADINT\n");
  }else if((refed->itp)->ttype == TPCHAR){
    fprintf(caslfilep,"\tCALL\tREADCHAR\n");
  }else{
    error("Couldn't read type read");
  }
  return 0;
}

int asm_readln(){
  fprintf(caslfilep,"\tCALL\tREADLINE\n");
  return 0;
}

int asm_write(){
  return 0;
}

int asm_writeln(){
  return 0;
}