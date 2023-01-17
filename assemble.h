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
int asm_read(ID * ref);
int asm_readln();


#endif