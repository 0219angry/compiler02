#include "token-list.h"

int if_true_label;
int if_false_label;

LSTACK * lstack = NULL;
extern int variable_address;
extern int is_need_new_address;

char programname_attr[MAXSTRSIZE];
CON * const_top = NULL;


int asm_start(char *programname){
  strcpy(programname_attr,programname);
  fprintf(caslfilep,"$$%s\tSTART\t\n",programname);
  fprintf(caslfilep,"\tLAD\tgr0, 0\n");
  fprintf(caslfilep,"\tCALL\t$block%%%s\n",programname);
  fprintf(caslfilep,"\tCALL\tFLUSH\n");
  fprintf(caslfilep,"\tSVC\t0\n");
  return 0;
}
int asm_end(){
  fprintf(caslfilep,"\tEND\t\n");
  return 0;
}

int asm_return_st(){
  fprintf(caslfilep,"\tRET\t\n");
  return 0;
}

int asm_val(ID * globalidloot){
  ID * id = globalidloot;
  while(id != NULL){
    if((id->itp)->ttype != TPPROC){
      fprintf(caslfilep,"$%s\tDC\t0\n",id->name);
    }
    id = id->nextp;
  }
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
    fprintf(caslfilep,"\tPUSH\t0, gr2\n");
  return 0;
}

int asm_proc_end(){
  fprintf(caslfilep,"\tRET\t\n");
  return 0;
}

int asm_block_start(){
  fprintf(caslfilep,"$block%%%s\tDS\t0\n",programname_attr);
  return 0;
}

int asm_ref_val(ID * refed){
  if(refed->ispara == 1){
    fprintf(caslfilep,"\tLD\tgr1, $%s%%%s\n",refed->name,refed->procname);
  }else if(refed->procname != NULL){
    fprintf(caslfilep,"\tLAD\tgr1, $%s%%%s\n",refed->name,refed->procname);
  }else{
    fprintf(caslfilep,"\tLAD\tgr1, $%s\n",refed->name);
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
  asm_ref_val(refed);
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tLD\tgr1, 0, gr1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_param_to_real(){
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tLD\tgr1, 0, gr1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_parameter(ID * refed){
  fprintf(caslfilep,"\tPOP\tgr1\n");
  if(refed->procname == NULL){
    fprintf(caslfilep,"\tLAD\tgr1, $%s\n",refed->name);
  }else{
    fprintf(caslfilep,"\tLAD\tgr1, $%s%%%s\n",refed->name, refed->procname);
  }
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_parameter_with_newlabel(){
  CON * top = const_top;
  CON * temp;
  char * string;
  if((string = malloc(sizeof(char)*(MAXSTRSIZE+12))) == NULL){
    return error("malloc error");
  }
  temp = malloc_CON();
  sprintf(string,"L%04d\tDC\t0\n",variable_address);
  label++;
  temp->string = string;
  if(top == NULL){
    const_top = temp;
    return 0;
  }
  while(top->nextp != NULL){
    top = top->nextp;
  }
  top->nextp = temp;
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tLAD\tgr2, L%04d\n",variable_address);
  fprintf(caslfilep,"\tST\tgr1, 0, gr2\n");
  fprintf(caslfilep,"\tPUSH\t0,gr2\n");
  variable_address = 0;
  is_need_new_address = 0;
  return 0;
}

int asm_if_st(){
  push(1);
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJZE\tL%04d\n",lstack->f);
  return 0;
}

int asm_else_st(){
  fprintf(caslfilep,"\tJUMP\tL%04d\n",lstack->end);
  fprintf(caslfilep,"L%04d",lstack->f);
  return 0;
}

int asm_if_st_end(){
  fprintf(caslfilep,"L%04d",lstack->end);
  pop();
  return 0;
}

int asm_ite_start(){
  push(2);
  fprintf(caslfilep,"L%04d",lstack->start);
  return 0;
}

int asm_ite_cmp(){
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJZE\tL%04d\n",lstack->end);
  return 0;
}

int asm_ite_continue(){
  fprintf(caslfilep,"\tJUMP\tL%04d\n",lstack->start);
  fprintf(caslfilep,"L%04d",lstack->end);
  pop();
  return 0;
}


int asm_array_count(){
  return 0;
}

int asm_number(){
  fprintf(caslfilep,"\tLAD\tgr1, %d\n",num_attr);
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_true(){
  fprintf(caslfilep,"\tLAD\tgr1, 1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_false(){
  fprintf(caslfilep,"\tLAD\tgr1, 0\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_char(){
  fprintf(caslfilep,"\tLAD\tgr1, %c\n",string_attr[0]);
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_string(){
  CON * temp;
  CON * top = const_top;
  char * string;
  fprintf(caslfilep,"\tLAD\tgr1, L%04d\n",label);
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  if((string = malloc(sizeof(char)*(MAXSTRSIZE+12))) == NULL){
    return error("malloc error");
  }
  temp = malloc_CON();
  sprintf(string,"L%04d\tDC\t'%s'\n",label,string_attr);
  label++;
  temp->string = string;
  if(top == NULL){
    const_top = temp;
    return 0;
  }
  while(top->nextp != NULL){
    top = top->nextp;
  }
  top->nextp = temp;
  return 0;
}

int asm_assign(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tST\tgr2,0,gr1\n");
  return 0;
}

int asm_expression(int opr){
  if_true_label = label;
  label++;
  if_false_label = label;
  label++;
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr2\n");
  switch(opr){
    case TEQUAL:
      fprintf(caslfilep,"\tJNZ\tL%04d\n",if_false_label);
      break;
    case TNOTEQ:
      fprintf(caslfilep,"\tJZE\tL%04d\n",if_false_label);
      break;
    case TLE:
      fprintf(caslfilep,"\tJPL\tL%04d\n",if_false_label);
      fprintf(caslfilep,"\tJZE\tL%04d\n",if_false_label);
      break;
    case TLEEQ:
      fprintf(caslfilep,"\tJPL\tL%04d\n",if_false_label);
      break;
    case TGR:
      fprintf(caslfilep,"\tJMI\tL%04d\n",if_false_label);
      fprintf(caslfilep,"\tJZE\tL%04d\n",if_false_label);
      break;
    case TGREQ:
      fprintf(caslfilep,"\tJMI\tL%04d\n",if_false_label);
      break;
    default:
      error("unknown relational operand");

  }
  fprintf(caslfilep,"\tLAD\tgr1, 1\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  fprintf(caslfilep,"\tJUMP\tL%04d\n",if_true_label);

  fprintf(caslfilep,"L%04d\tLD\tgr1, gr0\n",if_false_label);
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  fprintf(caslfilep,"L%04d",if_true_label);
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
  fprintf(caslfilep,"\tJOV\tEOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_SUBA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tSUBA\tgr1, gr2\n");
  fprintf(caslfilep,"\tJOV\tEOVF\n");
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
  fprintf(caslfilep,"\tJOV\tEOVF\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_DIVA(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tOR\tgr1,gr1\n");
  fprintf(caslfilep,"\tJZE\tE0DIV\n");
  fprintf(caslfilep,"\tDIVA\tgr1, gr2\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_AND(){
  fprintf(caslfilep,"\tPOP\tgr2\n");
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tAND\tgr1, gr2\n");
  fprintf(caslfilep,"\tPUSH\t0, gr1\n");
  return 0;
}

int asm_call(ID * called){
  fprintf(caslfilep,"\tCALL\t$%s\n",called->name);
  return 0;
}

int asm_read(ID * refed){
  fprintf(caslfilep,"\tPOP\tgr1\n");
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

int asm_output_format(int etype, int num){
  fprintf(caslfilep,"\tPOP\tgr1\n");
  fprintf(caslfilep,"\tLAD\tgr2, %d\n",num);
  switch(etype){
    case TPCHAR:
    case TPSTR:
      fprintf(caslfilep,"\tCALL\tWRITESTR\n");
      break;
    case TPINT:
      fprintf(caslfilep,"\tCALL\tWRITEINT\n");
      break;
    case TPBOOL:
      fprintf(caslfilep,"\tCALL\tWRITEBOOL\n");
      break;
  }
  return 0;
}

int asm_write(){
  fprintf(caslfilep,"\tCALL\tWRITE\n");
  return 0;
}

int asm_writeln(){
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  return 0;
}

int constants_output(){
  CON * top = const_top;

  while(top != NULL){
    fprintf(caslfilep,"%s",top->string);
    top = top->nextp;
  }
  return 0;
}

CON * malloc_CON(){
  CON * temp;
  temp = malloc(sizeof(CON));
  if(temp == NULL){
    error("malloc error");
    exit(EXIT_FAILURE);
  }
  temp->string = NULL;
  temp->nextp = NULL;
  return temp;
}


/* ラベル管理用のスタックに追加 */
/* type 1:if文,2:while文*/
LSTACK * push(int type){
  LSTACK * new;
  if((new=malloc(sizeof(LSTACK)))==NULL){
    error("malloc error");
    exit(EXIT_FAILURE);
  }
  if(type == 1){
    new->t = label;
    new->f = label+1;
    new->end = label+2;
    new->p = lstack;
    label = label+3;
  }else if(type == 2){
    new->start = label;
    new->end = label+1;
    new->p = lstack;
    label = label+2;
  }
  lstack = new;
  return lstack;
}

LSTACK * pop(){
  lstack = lstack->p;
  return lstack;
}