#include "token-list.h"

int isAlpha(int c);
int isNumber(int c);
int isSymbol(int c);
int getNewchar(void);

int num_attr;
char string_attr[MAXSTRSIZE];

int cbuf;

FILE * fp;

int init_scan(char *filename){
  if(fp = fopen(filename,"r") == NULL){
    return -1;
  }

  cbuf = fgetc(fp);
  return 0;
}

int scan(void){
  while(1){
    if(cbuf == ' ' || cbuf == '\t'){
      cbuf = fgetc(fp);
      continue;
    }else if(isAlpha(cbuf)){
      int i=0;
      while(isAlpha(cbuf)){
        string_attr[i] = cbuf;
        cbuf = fgetc(fp);
        if(!isAlpha(cbuf) || !isNumber(cbuf)){
          break;
        }
      }

    }else if(isNumber(cbuf)){
      while(isNumber(cbuf)){
        cbuf = fgetc(fp);
      }
    }else if(cbuf == '/'){
      cbuf = fgetc(fp);
      if(cbuf != '*'){
        return -1;
      }
      while(cbuf != '*'){
        cbuf = fgetc(fp);
      }

    }else if(cbuf == '{'){
      while(cbuf != '}'){
        cbuf = fgetc(fp);
      }
    }else if(isSymbol(cbuf)>0){
      
    }
  }
}



//for scan() functions

int isAlpha(int c){
  if((c>='a' && c<='z') || (c>='A' && c<='Z')){
    return 1;
  }else{
    return 0;
  }
}

int isNumber(int c){
  if(c>='0' && c<='9'){
    return 1;
  }else{
    return 0;
  }
}

int isSymbol(int c){
  
}


int get_linenum(void){
  return 0;
}

void end_scan(void){

}
