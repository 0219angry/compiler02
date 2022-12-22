#ifndef _H_SEMANTIC_
#define _H_SEMANTIC_


/* type */
#define TPINT 10
#define TPCHAR 11
#define TPBOOL 12
#define TPARRAY 13
#define TPARRAYINT 14
#define TPARRAYCHAR 15
#define TPARRAYBOOL 16
#define TPPROC 17

#define NUMOFTYPE 8


typedef struct TYPE_t {
  int ttype;
  int arraysize;
  struct TYPE_t *etp;
  struct TYPE_t *paratp;
} TYPE;

typedef struct LINE_t {
  int reflinenum;
  struct LINE_t *nextlinep;
} LINE;

typedef struct ID_t {
  char *name;
  char *procname;
  struct TYPE_t *itp;
  int ispara;
  int deflinenum;
  struct LINE_t *irefp;
  struct ID_t *nextp;
} ID;

typedef struct PROC_t {
  char *procname;
  struct ID_t *iidp;
  struct PROC_t *nextp;
} PROC;

void add_define_without_type(void);
void add_type(TYPE *ty);
TYPE * create_type(int ttype);
TYPE *add_formal_type(TYPE *loot, int ttype);
int regist_define(void);
ID * add_reference(char * name, int num);
int regist_proc_global();
int regist_proc_local();
ID * search_ID(ID * top, char * name);
int check_overloading(char * name);

ID * malloc_ID(void);
TYPE * malloc_TYPE(void);
PROC * malloc_PROC(void);
LINE * malloc_LINE(void);

void print_cridloot(void);
char * get_type_name(int ttype);

int print_ID_for_debug(ID * loot);



#endif