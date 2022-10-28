#include "token-list.h"

void init_attr(void);

int isAlpha(int c);
int isNumber(int c);
int isSymbol(int c);
int isKeyword(void);
void getNewchar_without_EOL(void);

int num_attr;
char string_attr[MAXSTRSIZE];

int cbuf;
int tokenbuf;
int line;

FILE * fp;

int init_scan(char *filename){
  if((fp = fopen(filename,"r")) == NULL){
    return -1;
  }
  cbuf = fgetc(fp);
  line = 1;
  init_attr();
  return 0;
}

void init_attr(void){
  num_attr = 0;
  for(int i=0;i<MAXSTRSIZE;i++){
    string_attr[i] = '\0';
  }
}

int scan(void){
  init_attr();
  if(cbuf == ' ' || cbuf == '\t'){
    cbuf = fgetc(fp);
    return 0;
  }else if(cbuf == '\n'){
    line++;
    cbuf = fgetc(fp);
    if(cbuf == '\r'){
      cbuf = fgetc(fp);
    }
    return 0;
  }else if(cbuf == '\r'){
    line++;
    cbuf = fgetc(fp);
    if(cbuf == '\n'){
      cbuf = fgetc(fp);
    }
    return 0;
  }else if(isAlpha(cbuf)){
    int i=0;
    while(isAlpha(cbuf) || isNumber(cbuf)){
      string_attr[i] = cbuf;
      i++;
      cbuf = fgetc(fp);
      if(!isAlpha(cbuf) && !isNumber(cbuf)){
        string_attr[i] = '\0';
        break;
      }
      if(i > MAXSTRSIZE){
        return -1;
      } 
    }
    return isKeyword();
  }else if(isNumber(cbuf)){
    int i=0;
    while(isNumber(cbuf)){
      string_attr[i] = cbuf;
      cbuf = fgetc(fp);
      i++;
    }
    string_attr[i] = '\0';
    num_attr = atoi(string_attr);
    return TINTEGER;
  }else if(cbuf == '/'){
    cbuf = fgetc(fp);
    printf("%c",cbuf);
    if(cbuf != '*'){
      return -1;
    }
    cbuf = fgetc(fp);
    while(1){
      if(cbuf == '*'){
        cbuf = fgetc(fp);
        if(cbuf == '/'){  
          cbuf = fgetc(fp);
          return 0;
        }else{
          continue;
        }
      }else if(cbuf == EOF){
        return -1;
      }else{
        cbuf = fgetc(fp);
        continue;
      } 
    }
  }else if(cbuf == '{'){
    while(cbuf != '}'){
      if(cbuf == EOF){
        return -1;
      }
      getNewchar_without_EOL();
    }
    cbuf = fgetc(fp);
    return 0;
  }else if(cbuf == '\''){
    int i=0;
    cbuf = fgetc(fp);
    while(1){
      if(cbuf == EOF){
        return -1;
      }else if(cbuf == '\n'){
        return -1;
      }else if(cbuf == '\''){
        cbuf = fgetc(fp);
        if(cbuf == '\''){
          string_attr[i] = '\'';
          i++;
          string_attr[i] = '\'';
          i++;
        }else{
          string_attr[i] = '\0';
          return TSTRING;
        }
      }
      string_attr[i] = cbuf;
      cbuf = fgetc(fp);
      i++;
    }
  }else if((tokenbuf = isSymbol(cbuf))>0){
    if(tokenbuf == TLE || tokenbuf == TGR || tokenbuf == TCOLON){

    }else{
      cbuf = fgetc(fp);
    }
    return tokenbuf;
  }else if(cbuf == EOF){
    return -2;
  }
  return -1;
}




//for scan() functions

void getNewchar_without_EOL(void){
  cbuf = fgetc(fp);
  if(cbuf == '\r'){
    cbuf = fgetc(fp);
    line++;
    if(cbuf == '\n'){
      cbuf = fgetc(fp);
    }
  }else if(cbuf == '\n'){
    cbuf = fgetc(fp);
    if(cbuf == '\r'){
      cbuf = fgetc(fp);
    }
  }
}


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
  switch(c){
    case '+': return TPLUS;
    case '-': return TMINUS;
    case '*': return TSTAR;
    case '=': return TEQUAL;
    case '(': return TLPAREN;
    case ')': return TRPAREN;
    case '[': return TLSQPAREN;
    case ']': return TRSQPAREN;
    case '.': return TDOT;
    case ',': return TCOMMA;
    case ';': return TSEMI;
    case '<':
      cbuf = fgetc(fp); 
      if(cbuf == '>'){
        return TNOTEQ;
      }else if(cbuf == '='){
        return TLEEQ;
      }else{
        return TLE;
      }
    case '>': 
      cbuf = fgetc(fp);
      if(cbuf == '='){
        return TGREQ;
      }else{
        return TGR;
      }
    case ':':
      cbuf = fgetc(fp);
      if(cbuf == '='){
        return TASSIGN;
      }else{
        return TCOLON;
      }
    default : return -1;
  }
}

int isKeyword(void){
  for(int i=0;i<KEYWORDSIZE;i++){
    if(strcmp(string_attr,key[i].keyword) == 0){
      return key[i].keytoken;
    }
  }
  return TNAME;
}


int get_linenum(void){
  return line;
}

void end_scan(void){
  fclose(fp);
}
