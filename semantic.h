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

typedef struct TYPE_t {
  int ttype;
  int arraysize;
  TYPE *etp;
  TYPE *paratp;
} TYPE;

typedef struct LINE_t {
  int reflinenum;
  LINE *nextlinep;
} LINE;

typedef struct ID_t {
  char *name;
  char *procname;
  TYPE *itp;
  int ispara;
  int deflinenum;
  LINE *irefp;
  ID *nextp;
} ID;

void init_semantic(void);
void clear_temp(void);
ID * add_define(void);
ID * add_reference(char * name, int num);
ID * search_ID(ID * top, char * name);


#endif