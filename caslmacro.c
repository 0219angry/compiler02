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
  fprintf(caslfilep,"WB2\tCALL\tWRITESTR\n");
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

void outlib(void) {
  fprintf(caslfilep,""
"; ------------------------\n"
"; Utility functions\n"
"; ------------------------\n"
"EOVF            CALL    WRITELINE\n"
"                LAD     gr1, EOVF1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     1  ;  overflow error stop\n"
"EOVF1           DC      '***** Run-Time Error : Overflow *****'\n"
"E0DIV           JNZ     EOVF\n"
"                CALL    WRITELINE\n"
"                LAD     gr1, E0DIV1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     2  ;  0-divide error stop\n"
"E0DIV1          DC      '***** Run-Time Error : Zero-Divide *****'\n"
"EROV            CALL    WRITELINE\n"
"                LAD     gr1, EROV1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     3  ;  range-over error stop\n"
"EROV1           DC      '***** Run-Time Error : Range-Over in Array Index *****'\n"
"; gr1の値（文字）をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITECHAR       RPUSH\n"
"                LD      gr6, SPACE\n"
"                LD      gr7, OBUFSIZE\n"
"WC1             SUBA    gr2, ONE  ; while(--c > 0) {\n"
"                JZE     WC2\n"
"                JMI     WC2\n"
"                ST      gr6, OBUF,gr7  ;  *p++ = ' ';\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WC1  ; }\n"
"WC2             ST      gr1, OBUF,gr7  ; *p++ = gr1;\n"
"                CALL    BOVFCHECK\n"
"                ST      gr7, OBUFSIZE\n"
"                RPOP\n"
"                RET\n"
"; gr1が指す文字列をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITESTR        RPUSH\n"
"                LD      gr6, gr1  ; p = gr1;\n"
"WS1             LD      gr4, 0,gr6  ; while(*p != 0) {\n"
"                JZE     WS2\n"
"                ADDA    gr6, ONE  ;  p++;\n"
"                SUBA    gr2, ONE  ;  c--;\n"
"                JUMP    WS1  ; }\n"
"WS2             LD      gr7, OBUFSIZE  ; q = OBUFSIZE;\n"
"                LD      gr5, SPACE\n"
"WS3             SUBA    gr2, ONE  ; while(--c >= 0) {\n"
"                JMI     WS4\n"
"                ST      gr5, OBUF,gr7  ;  *q++ = ' ';\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WS3  ; }\n"
"WS4             LD      gr4, 0,gr1  ; while(*gr1 != 0) {\n"
"                JZE     WS5\n"
"                ST      gr4, OBUF,gr7  ;  *q++ = *gr1++;\n"
"                ADDA    gr1, ONE\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WS4  ; }\n"
"WS5             ST      gr7, OBUFSIZE  ; OBUFSIZE = q;\n"
"                RPOP\n"
"                RET\n"
"BOVFCHECK       ADDA    gr7, ONE\n"
"                CPA     gr7, BOVFLEVEL\n"
"                JMI     BOVF1\n"
"                CALL    WRITELINE\n"
"                LD      gr7, OBUFSIZE\n"
"BOVF1           RET\n"
"BOVFLEVEL       DC      256\n"
"; gr1の値（整数）をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITEINT        RPUSH\n"
"                LD      gr7, gr0  ; flag = 0;\n"
"                CPA     gr1, gr0  ; if(gr1>=0) goto WI1;\n"
"                JPL     WI1\n"
"                JZE     WI1\n"
"                LD      gr4, gr0  ; gr1= - gr1;\n"
"                SUBA    gr4, gr1\n"
"                CPA     gr4, gr1\n"
"                JZE     WI6\n"
"                LD      gr1, gr4\n"
"                LD      gr7, ONE  ; flag = 1;\n"
"WI1             LD      gr6, SIX  ; p = INTBUF+6;\n"
"                ST      gr0, INTBUF,gr6  ; *p = 0;\n"
"                SUBA    gr6, ONE  ; p--;\n"
"                CPA     gr1, gr0  ; if(gr1 == 0)\n"
"                JNZ     WI2\n"
"                LD      gr4, ZERO  ;  *p = '0';\n"
"                ST      gr4, INTBUF,gr6\n"
"                JUMP    WI5  ; }\n"
"; else {\n"
"WI2             CPA     gr1, gr0  ;  while(gr1 != 0) {\n"
"                JZE     WI3\n"
"                LD      gr5, gr1  ;   gr5 = gr1 - (gr1 / 10) * 10;\n"
"                DIVA    gr1, TEN  ;   gr1 /= 10;\n"
"                LD      gr4, gr1\n"
"                MULA    gr4, TEN\n"
"                SUBA    gr5, gr4\n"
"                ADDA    gr5, ZERO  ;   gr5 += '0';\n"
"                ST      gr5, INTBUF,gr6  ;   *p = gr5;\n"
"                SUBA    gr6, ONE  ;   p--;\n"
"                JUMP    WI2  ;  }\n"
"WI3             CPA     gr7, gr0  ;  if(flag != 0) {\n"
"                JZE     WI4\n"
"                LD      gr4, MINUS  ;   *p = '-';\n"
"                ST      gr4, INTBUF,gr6\n"
"                JUMP    WI5  ;  }\n"
"WI4             ADDA    gr6, ONE  ;  else p++;\n"
"; }\n"
"WI5             LAD     gr1, INTBUF,gr6  ; gr1 = p;\n"
"                CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"WI6             LAD     gr1, MMINT\n"
"                CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"MMINT           DC      '-32768'\n"
"; gr1の値（真理値）が0なら'FALSE'を\n"
"; 0以外なら'TRUE'をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITEBOOL       RPUSH\n"
"                CPA     gr1, gr0  ; if(gr1 != 0)\n"
"                JZE     WB1\n"
"                LAD     gr1, WBTRUE  ;  gr1 = TRUE;\n"
"                JUMP    WB2\n"
"; else\n"
"WB1             LAD     gr1, WBFALSE  ;  gr1 = FALSE;\n"
"WB2             CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"WBTRUE          DC      'TRUE'\n"
"WBFALSE         DC      'FALSE'\n"
"; 改行を出力する\n"
"WRITELINE       RPUSH\n"
"                LD      gr7, OBUFSIZE\n"
"                LD      gr6, NEWLINE\n"
"                ST      gr6, OBUF,gr7\n"
"                ADDA    gr7, ONE\n"
"                ST      gr7, OBUFSIZE\n"
"                OUT     OBUF, OBUFSIZE\n"
"                ST      gr0, OBUFSIZE\n"
"                RPOP\n"
"                RET\n"
"FLUSH           RPUSH\n"
"                LD      gr7, OBUFSIZE\n"
"                JZE     FL1\n"
"                CALL    WRITELINE\n"
"FL1             RPOP\n"
"                RET\n"
"; gr1が指す番地に文字一つを読み込む\n"
"READCHAR        RPUSH\n"
"                LD      gr5, RPBBUF  ; if(RPBBUF != 0) {\n"
"                JZE     RC0\n"
"                ST      gr5, 0,gr1  ;  *gr1 = RPBBUF;\n"
"                ST      gr0, RPBBUF  ;  RPBBUF = 0\n"
"                JUMP    RC3  ;  return; }\n"
"RC0             LD      gr7, INP  ; inp = INP;\n"
"                LD      gr6, IBUFSIZE  ; if(IBUFSIZE == 0) {\n"
"                JNZ     RC1\n"
"                IN      IBUF, IBUFSIZE  ;  IN();\n"
"                LD      gr7, gr0  ;  inp = 0;\n"
"; }\n"
"RC1             CPA     gr7, IBUFSIZE  ; if(inp == IBUFSIZE) {\n"
"                JNZ     RC2\n"
"                LD      gr5, NEWLINE  ;  *gr1 = '\\n';\n"
"                ST      gr5, 0,gr1\n"
"                ST      gr0, IBUFSIZE  ;  IBUFSIZE = INP = 0;\n"
"                ST      gr0, INP\n"
"                JUMP    RC3  ; }\n"
"; else {\n"
"RC2             LD      gr5, IBUF,gr7  ;  *gr1 = *inp++;\n"
"                ADDA    gr7, ONE\n"
"                ST      gr5, 0,gr1\n"
"                ST      gr7, INP  ;  INP = inp;\n"
"; }\n"
"RC3             RPOP\n"
"                RET\n"
"; gr1が指す番地に整数値一つを読み込む\n"
"READINT         RPUSH\n"
"; do {\n"
"RI1             CALL    READCHAR  ;  ch = READCHAR();\n"
"                LD      gr7, 0,gr1\n"
"                CPA     gr7, SPACE  ; } while(ch==' ' || ch=='\\t' || ch=='\\n');\n"
"                JZE     RI1\n"
"                CPA     gr7, TAB\n"
"                JZE     RI1\n"
"                CPA     gr7, NEWLINE\n"
"                JZE     RI1\n"
"                LD      gr5, ONE  ; flag = 1\n"
"                CPA     gr7, MINUS  ; if(ch == '-') {\n"
"                JNZ     RI4\n"
"                LD      gr5, gr0  ;  flag = 0;\n"
"                CALL    READCHAR  ;  ch = READCHAR();\n"
"                LD      gr7, 0,gr1\n"
"RI4             LD      gr6, gr0  ; v = 0;     ; }\n"
"RI2             CPA     gr7, ZERO  ; while('0' <= ch && ch <= '9') {\n"
"                JMI     RI3\n"
"                CPA     gr7, NINE\n"
"                JPL     RI3\n"
"                MULA    gr6, TEN  ;  v = v*10+ch-'0';\n"
"                ADDA    gr6, gr7\n"
"                SUBA    gr6, ZERO\n"
"                CALL    READCHAR  ;  ch = READSCHAR();\n"
"                LD      gr7, 0,gr1\n"
"                JUMP    RI2  ; }\n"
"RI3             ST      gr7, RPBBUF  ; ReadPushBack();\n"
"                ST      gr6, 0,gr1  ; *gr1 = v;\n"
"                CPA     gr5, gr0  ; if(flag == 0) {\n"
"                JNZ     RI5\n"
"                SUBA    gr5, gr6  ;  *gr1 = -v;\n"
"                ST      gr5, 0,gr1\n"
"; }\n"
"RI5             RPOP\n"
"                RET\n"
"; 入力を改行コードまで（改行コードも含む）読み飛ばす\n"
"READLINE        ST      gr0, IBUFSIZE\n"
"                ST      gr0, INP\n"
"                ST      gr0, RPBBUF\n"
"                RET\n"
"ONE             DC      1\n"
"SIX             DC      6\n"
"TEN             DC      10\n"
"SPACE           DC      #0020  ; ' '\n"
"MINUS           DC      #002D  ; '-'\n"
"TAB             DC      #0009  ; '\\t'\n"
"ZERO            DC      #0030  ; '0'\n"
"NINE            DC      #0039  ; '9'\n"
"NEWLINE         DC      #000A  ; '\\n'\n"
"INTBUF          DS      8\n"
"OBUFSIZE        DC      0\n"
"IBUFSIZE        DC      0\n"
"INP             DC      0\n"
"OBUF            DS      257\n"
"IBUF            DS      257\n"
"RPBBUF          DC      0\n"
  );
}