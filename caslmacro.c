#include "token-list.h"



int open_caslfile(char * name){
  int i,len;
  len = strlen(name);
  char * outputname = malloc(sizeof(char) * len);
  for(i=0;i<len;i++){
    if(name[i] == '.'){
      outputname[i] = '.';
      outputname[i+1] = 'c';
      outputname[i+2] = 's';
      outputname[i+3] = 'l';
      break;
    }
    outputname[i] = name[i];
  }
  if((caslfilep = fopen(outputname,"w")) == NULL){
    return -1;
  }
  label = 1;
  return 0;
}

void add_utils(){
  fprintf(caslfilep," ;                           ;\n");
  fprintf(caslfilep," ;     utilities library     ;\n");
  fprintf(caslfilep," ;                           ;\n");
  add_EOVF();
  add_E0DIV();
  add_EROV();
  add_WRITECHAR();
  add_WRITESTR();
  add_BOVFCHECK();
  add_WRITEINT();
  add_WRITEBOOL();
  add_WRITELINE();
  add_FLUSH();
  add_READCHAR();
  add_READINT();
  add_READLINE();
  add_const();
  fprintf(caslfilep,"\tEND\t\n");
}


/* エラー処理系コード */

/* Runtime error : Overflow */
void add_EOVF(){
  fprintf(caslfilep,"EOVF\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tLAD\tgr1, EOVF1\n");
  fprintf(caslfilep,"\tLD\tgr2, gr0\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tSVC\t1\t; overflow error stop\n");
  fprintf(caslfilep,"EOVF1\tDC\t'***** Run-Time Error : Overflow *****'\n");
}

/* Runtime error : 0-divide */
void add_E0DIV(){
  fprintf(caslfilep,"E0DIV\tJNZ\tEOVF\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tLAD\tgr1, E0DIV\n");
  fprintf(caslfilep,"\tLD\tgr2, gr0\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tSVC\t2\t; 0-divide error stop\n");
  fprintf(caslfilep,"E0DIV1\tDC\t'***** Run-Time Error : Zero-Divide ******'\n");
}


/* Runtime error : Range-Over in Array Index */
void add_EROV(){
  fprintf(caslfilep,"EROV\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tLAD\tgr1, EROV1\n");
  fprintf(caslfilep,"\tLD\tgr2, gr0\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tSVC\t3\t; range-over error stop\n");
  fprintf(caslfilep,"EROV1\tDC\t'***** Run-Time Error : Range-Over in Array Index *****'\n");
}

/* gr1の値(文字)をgr2の桁数で出力する */
void add_WRITECHAR(){
  fprintf(caslfilep,"WRITECHAR\tRPUSH\t\n");
  fprintf(caslfilep,"\tLD\tgr6, SPACE\n");
  fprintf(caslfilep,"\tLD\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"WC1\tSUBA\tgr2, ONE\n");
  fprintf(caslfilep,"\tJZE\tWC2\n");
  fprintf(caslfilep,"\tJMI\tWC2\n");
  fprintf(caslfilep,"\tST\tgr1, OBUF, gr7\n");
  fprintf(caslfilep,"\tCALL\tBOVFCHECK\n");
  fprintf(caslfilep,"\tJUMP\tWC1\n");
  fprintf(caslfilep,"WC2\tST\tgr1, OBUF, gr7\n");
  fprintf(caslfilep,"\tCALL\tBOVFCHECK\n");
  fprintf(caslfilep,"\tST\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

/* gr1が指す文字列をgr2の桁数で出力する */
void add_WRITESTR(){
  fprintf(caslfilep,"WRITESTR\tRPUSH\t\n");
  fprintf(caslfilep,"\tLD\tgr6, gr1\n");
  fprintf(caslfilep,"WS1\tLD\tgr4, 0, gr6\n");
  fprintf(caslfilep,"\tJZE\tWS2\n");
  fprintf(caslfilep,"\tADDA\tgr6, ONE\n");
  fprintf(caslfilep,"\tSUBA\tgr2, ONE\n");
  fprintf(caslfilep,"\tJUMP\tWS1\n");
  fprintf(caslfilep,"WS2\tLD\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tLD\tgr5, SPACE\n");
  fprintf(caslfilep,"WS3\tSUBA\tgr2, ONE\n");
  fprintf(caslfilep,"\tJMI\tWS4\n");
  fprintf(caslfilep,"\tST\tgr5, OBUF, gr7\n");
  fprintf(caslfilep,"\tCALL\tBOVFCHECK\n");
  fprintf(caslfilep,"\tJUMP\tWS3\n");
  fprintf(caslfilep,"WS4\tLD\tgr4, 0, gr1\n");
  fprintf(caslfilep,"\tJZE\tWS5\n");
  fprintf(caslfilep,"\tST\tgr4, OBUF, gr7\n");
  fprintf(caslfilep,"\tADDA\tgr1, ONE\n");
  fprintf(caslfilep,"\tJUMP\tWS4\n");
  fprintf(caslfilep,"WS5\tST\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

void add_BOVFCHECK(){
  fprintf(caslfilep,"BOVFCHECK\tADDA\tgr7, ONE\n");
  fprintf(caslfilep,"\tCPA\tgr7, BOVFLEVEL\n");
  fprintf(caslfilep,"\tJMI\tBOVF1\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"\tLD\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"BOVF1\tRET\t\n");
  fprintf(caslfilep,"BOVFLEVEL\tDC\t256\n");
}

/* gr1の値(整数)をgr2の桁数で出力する */
void add_WRITEINT(){
  fprintf(caslfilep,"WRITEINT\tRPUSH\t\n");
  fprintf(caslfilep,"\tLD\tgr7, gr0\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJPL\tWI1\n");
  fprintf(caslfilep,"\tJZE\tWI1\n");
  fprintf(caslfilep,"\tLD\tgr4, gr0\n");
  fprintf(caslfilep,"\tSUBA\tgr4, gr1\n");
  fprintf(caslfilep,"\tCPA\tgr4, gr1\n");
  fprintf(caslfilep,"\tJZE\tWI6\n");
  fprintf(caslfilep,"\tLD\tgr1,gr4\n");
  fprintf(caslfilep,"\tLD\tgr7,ONE\n");
  fprintf(caslfilep,"WI1\tLD\tgr6, SIX\n");
  fprintf(caslfilep,"\tST\tgr0, INTBUF, gr6\n");
  fprintf(caslfilep,"\tSUBA\tgr6, ONE\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJNZ\tWI2\n");
  fprintf(caslfilep,"\tLD\tgr4, ZERO\n");
  fprintf(caslfilep,"\tST\tgr4, INTBUF, gr6\n");
  fprintf(caslfilep,"\tJUMP\tWI5\n");
  fprintf(caslfilep,"WI2\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJZE\tWI3\n");
  fprintf(caslfilep,"\tLD\tgr5,gr1\n");
  fprintf(caslfilep,"\tDIVA\tgr1, TEN\n");
  fprintf(caslfilep,"\tLD\tgr4, gr1\n");
  fprintf(caslfilep,"\tMULA\tgr4, TEN\n");
  fprintf(caslfilep,"\tSUBA\tgr5, gr4\n");
  fprintf(caslfilep,"\tADDA\tgr5, ZERO\n");
  fprintf(caslfilep,"\tST\tgr5, INTBUF, gr6\n");
  fprintf(caslfilep,"\tSUBA\tgr6, ONE\n");
  fprintf(caslfilep,"\tJUMP\tWI2\n");
  fprintf(caslfilep,"WI3\tCPA\tgr7, gr0\n");
  fprintf(caslfilep,"\tJZE\tWI4\n");
  fprintf(caslfilep,"\tLD\tgr4, MINUS\n");
  fprintf(caslfilep,"\tST\tgr4, INTBUF, gr6\n");
  fprintf(caslfilep,"\tJUMP WI5\t\n");
  fprintf(caslfilep,"WI4\tADDA\tgr6, ONE\n");
  fprintf(caslfilep,"WI5\tLAD\tgr1, INTBUF, gr6\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
  fprintf(caslfilep,"WI6\tLAD\tgr1, MMINT\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
  fprintf(caslfilep,"MMINT\tDC\t'-32768'\n");
}

/* gr1の値(真理値)が0なら'FALSEを0以外なら'TRUE'をgr2の桁数で出力する */
void add_WRITEBOOL(){
  fprintf(caslfilep,"WRITEBOOL\tRPUSH\t\n");
  fprintf(caslfilep,"\tCPA\tgr1, gr0\n");
  fprintf(caslfilep,"\tJZE\tWB1\n");
  fprintf(caslfilep,"\tLAD\tgr1, WBTRUE\n");
  fprintf(caslfilep,"\tJUMP\tWB2\n");
  fprintf(caslfilep,"WB1\tLAD\tgr1, WBFALSE\n");
  fprintf(caslfilep,"\tCALL\tWRITESTR\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
  fprintf(caslfilep,"WBTRUE\tDC\t'TRUE'\n");
  fprintf(caslfilep,"WBFALSE\tDC\t'FALSE'\n");
}

/* 改行を出力する */
void add_WRITELINE(){
  fprintf(caslfilep,"WRITELINE\tRPUSH\t\n");
  fprintf(caslfilep,"\t\t\n");
  fprintf(caslfilep,"\tLD\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tLD\tgr6, NEWLINE\n");
  fprintf(caslfilep,"\tST\tgr6, OBUF, gr7\n");
  fprintf(caslfilep,"\tADDA\tgr7, ONE\n");
  fprintf(caslfilep,"\tST\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tOUT\tOBUF, OBUFSIZE\n");
  fprintf(caslfilep,"\tST\tgr0, OBUFSIZE\n");
  fprintf(caslfilep,"\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

void add_FLUSH(){
  fprintf(caslfilep,"FLUSH\tRPUSH\t\n");
  fprintf(caslfilep,"\tLD\tgr7, OBUFSIZE\n");
  fprintf(caslfilep,"\tJZE\tFL1\n");
  fprintf(caslfilep,"\tCALL\tWRITELINE\n");
  fprintf(caslfilep,"FL1\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

/* gr1がさす番地に文字１つを読み込む */
void add_READCHAR(){
  fprintf(caslfilep,"READCHAR\tRPUSH\t\n");
  fprintf(caslfilep,"\tLD\tgr5, RPBBUF\n");
  fprintf(caslfilep,"\tJZE\tRC0\n");
  fprintf(caslfilep,"\tST\tgr5, 0, gr1\n");
  fprintf(caslfilep,"\tST\tgr0, RPBBUF\n");
  fprintf(caslfilep,"\tJUMP\tRC3\n");
  fprintf(caslfilep,"RC0\tLD\tgr7, INP\n");
  fprintf(caslfilep,"\tLD\tgr6, IBUFSIZE\n");
  fprintf(caslfilep,"\tJNZ\tRC1\n");
  fprintf(caslfilep,"\tIN\tIBUF, IBUFSIZE\n");
  fprintf(caslfilep,"\tLD\tgr7, gr0\n");
  fprintf(caslfilep,"RC1\tCPA\tgr7, IBUFSIZE\n");
  fprintf(caslfilep,"\tJNZ\tRC2\n");
  fprintf(caslfilep,"\tLD\tRC2\n");
  fprintf(caslfilep,"\tLD\tgr5, NEWLINE\n");
  fprintf(caslfilep,"\tST\tgr5, 0, gr1\n");
  fprintf(caslfilep,"\tST\tgr0, IBUFSIZE\n");
  fprintf(caslfilep,"\tST\tgr0, INP\n");
  fprintf(caslfilep,"\tJUMP\tRC3\n");
  fprintf(caslfilep,"RC2\tLD\tgr5, IBUF, gr7\n");
  fprintf(caslfilep,"\tADDA\tgr7, ONE\n");
  fprintf(caslfilep,"\tST\tgr5, 0, gr1\n");
  fprintf(caslfilep,"\tST\tgr7, INP\n");
  fprintf(caslfilep,"RC3\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

/* gr1がさす番地に整数値１つを読み込む */
void add_READINT(){
  fprintf(caslfilep,"READINT\tRPUSH\t\n");
  fprintf(caslfilep,"RI1\tCALL\tREADCHAR\n");
  fprintf(caslfilep,"\tLD\tgr7, 0, gr1\n");
  fprintf(caslfilep,"\tCPA\tgr7, SPACE\n");
  fprintf(caslfilep,"\tJZE\tRI1\n");
  fprintf(caslfilep,"\tCPA\tgr7, TAB\n");
  fprintf(caslfilep,"\tJZE\tRI1\n");
  fprintf(caslfilep,"\tCPA\tgr7, NEWLINE\n");
  fprintf(caslfilep,"\tJZE\tRI1\n");
  fprintf(caslfilep,"\tLD\tgr5, ONE\n");
  fprintf(caslfilep,"\tCPA\tgr7, MINUS\n");
  fprintf(caslfilep,"\tJNZ\tRI4\n");
  fprintf(caslfilep,"\tLD gr5, gr0\t\n");
  fprintf(caslfilep,"\tCALL\tREADCHAR\n");
  fprintf(caslfilep,"\tLD\tgr7, 0, gr1\n");
  fprintf(caslfilep,"RI4\tLD\tgr6, gr0\n");
  fprintf(caslfilep,"RI2\tCPA\tgr7, ZERO\n");
  fprintf(caslfilep,"\tJMI\tRI3\n");
  fprintf(caslfilep,"\tCPA\tgr7, NINE\n");
  fprintf(caslfilep,"\tJPL\tRI3\n");
  fprintf(caslfilep,"\tMULA\tgr6, TEN\n");
  fprintf(caslfilep,"\tADDA\tgr6, gr7\n");
  fprintf(caslfilep,"\tSUBA\tgr6, ZERO\n");
  fprintf(caslfilep,"\tCALL\tREADCHAR\n");
  fprintf(caslfilep,"\tLD\tgr7, 0, gr1\n");
  fprintf(caslfilep,"\tJUMP\tRI2\n");
  fprintf(caslfilep,"RI3\tST\tgr7, RPBBUF\n");
  fprintf(caslfilep,"\tST\tgr6, 0, gr1\n");
  fprintf(caslfilep,"\tCPA\tgr5, gr0\n");
  fprintf(caslfilep,"\tJNZ\tRI5\n");
  fprintf(caslfilep,"\tSUBA\tgr5, gr6\n");
  fprintf(caslfilep,"\tST\tgr5, 0, gr1\n");
  fprintf(caslfilep,"RI5\tRPOP\t\n");
  fprintf(caslfilep,"\tRET\t\n");
}

/* 入力を改行コードまで(改行コード含む)読み飛ばす */
void add_READLINE(){
  fprintf(caslfilep,"READLINE\tST\tgr0, IBUFSIZE\n");
  fprintf(caslfilep,"\tST\tgr0, INP\n");
  fprintf(caslfilep,"\tST\tgr0, RPBBUF\n");
  fprintf(caslfilep,"\tRET\t\n");
}

void add_const(){
  fprintf(caslfilep,"ONE\tDC\t1\n");
  fprintf(caslfilep,"SIX\tDC\t6\n");
  fprintf(caslfilep,"TEN\tDC\t10\n");
  fprintf(caslfilep,"SPACE\tDC\t#0020\n");
  fprintf(caslfilep,"MINUS\tDC\t#002D\n");
  fprintf(caslfilep,"TAB\tDC\t#0009\n");
  fprintf(caslfilep,"ZERO\tDC\t#0030\n");
  fprintf(caslfilep,"NINE\tDC\t#0039\n");
  fprintf(caslfilep,"NEWLINE\tDC\t#000A\n");
  fprintf(caslfilep,"INTBUF\tDS\t8\n");
  fprintf(caslfilep,"OBUFSIZE\tDC\t0\n");
  fprintf(caslfilep,"IBUFSIZE\tDC\t0\n");
  fprintf(caslfilep,"INP\tDC\t0\n");
  fprintf(caslfilep,"OBUF\tDS\t257\n");
  fprintf(caslfilep,"IBUF\tDS\t257\n");
  fprintf(caslfilep,"RPBBUF\tDC\t0\n");
}
