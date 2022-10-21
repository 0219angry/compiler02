#include "token-list.h"

int num_attr;
char string_attr[MAXSTRSIZE];

FILE * fp;

int init_scan(char *filename){
  if(fp = fopen(filename,"r") == NULL){
    return -1;
  }
  return 0;
}

int scan(void){

}

int get_linenum(void){

}

void end_scan(void){

}
