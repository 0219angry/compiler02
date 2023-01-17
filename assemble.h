#ifndef _H_ASSEMBLE
#define _H_ASSEMBLE

FILE * caslfilep;
int label;

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

/* assemble.c */
int asm_start(char * programname);
int asm_return_st();
int asm_proc_val(ID * localidloot);
int asm_proc_start(ID * localidloot, char * procname);
int asm_ref_lval(ID * ref);
int asm_ref_rval(ID * ref);
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


#endif