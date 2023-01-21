#ifndef _H_ASSEMBLE
#define _H_ASSEMBLE

FILE * caslfilep;
int label;


/* struct */

typedef struct CON_t {
  char * string;
  struct CON_t * nextp;
} CON;

typedef struct LSTACK_t {
  int start; // 開始位置のラベル番号(while文)
  int t; // 真の処理位置のラベル番号(if文)
  int f; // 偽の処理位置のラベル番号(if文)
  int end; // 終了位置のラベル番号(if文・while文)
  struct LSTACK_t * p;
} LSTACK;

/* caslmacro.c */
int open_caslfile(char * name);
void add_utils();
void add_EOVF();
void add_E0DIV();
void add_EROV();
void add_WRITECHAR();
void add_WRITESTR();
void add_BOVFCHECK();
void add_WRITEINT();
void add_WRITEBOOL();
void add_WRITELINE();
void add_FLUSH();
void add_READCHAR();
void add_READINT();
void add_READLINE();
void add_const();

void outlib();

/* assemble.c */
int asm_start(char * programname);
int asm_end();
int asm_return_st();
int asm_proc_val(ID * localidloot);
int asm_val(ID * globalidloot);
int asm_proc_start(ID * localidloot, char * procname);
int asm_proc_end();

int asm_block_start();
int asm_parameter(ID * ref);
int asm_parameter_with_newlabel();
int asm_ref_val(ID * ref);
int asm_ref_rval(ID * ref);
int asm_param_to_real();

int asm_if_st();
int asm_else_st();
int asm_if_st_end();

int asm_ite_start();
int asm_ite_cmp();
int asm_ite_continue();

int asm_number();
int asm_true();
int asm_false();
int asm_char();
int asm_string();


int asm_assign();

int asm_expression(int opr);

int asm_minus_sign();
int asm_ADDA();
int asm_SUBA();
int asm_OR();
int asm_MULA();
int asm_DIVA();
int asm_AND();

int asm_call(ID * called);
int asm_read(ID * ref);
int asm_readln();
int asm_output_format(int etype, int num);
int asm_write();
int asm_writeln();

int constants_output();

CON * malloc_CON();

LSTACK * push(int type);
LSTACK * pop();


#endif