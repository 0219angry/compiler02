#include "token-list.h"

int isSeparator(void);
int isSymbol(void);
int getNewchar(void);

int num_attr;
char string_attr[MAXSTRSIZE];

int cbuf[2];

FILE * fp;

int init_scan(char *filename){
  if(fp = fopen(filename,"r") == NULL){
    return -1;
  }
  return 0;
}

int scan(void){
  getNewchar();
  if(getNewchar() == -1){
    return -1;
  }
  
}

//for scan() functions

int isSeparator(void){

}

int isSymbol(void){
  switch(cbuf[0]){
    case '+': return TPLUS;
    case '-': return TMINUS;
    case '*': return TSTAR;
    case '=': return TEQUAL;
    case '<': if(getNewchar() != 0){
        if(cbuf[1] == '>'){
          getNewchar();
          return TNOTEQ;
        }else if(cbuf[1] == '='){
          getNewchar();
          return TLEEQ;
        }else{
          return TLE;
        }
      }
    case '>': if(getNewchar() != 0){
        if(cbuf[1] == '='){
          getNewchar();
          return TGREQ;
        }else{
          return TGR;
        }
      }
    case '(': return TLPAREN;
    case ')': return TRPAREN;
    case '[': return TLSQPAREN;
    case ']': return TRSQPAREN;
    case '.': return TDOT;
    case ',': return TCOMMA;
    case ':': if(getNewchar() != 0){
        if(cbuf[1] == '='){
          getNewchar();
          return TASSIGN;
        }else{
          return TCOLON;
        }
      }
    case ';': return TSEMI;
    default : return -1;
  }
}

int getNewchar(void){
  cbuf[0] = cbuf[1];
  if(cbuf[0] == EOF){
    return -1;
  }
  cbuf[1] = fgetc(fp);
  return 0;
}

int get_linenum(void){

}

void end_scan(void){

}
