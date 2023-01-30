#include "token-list.h"


/* keyword list */
struct KEY key[KEYWORDSIZE] = {
	{"and", 	TAND	},
	{"array",	TARRAY	},
	{"begin",	TBEGIN	},
	{"boolean",	TBOOLEAN},
	{"break",	TBREAK  },
	{"call",	TCALL	},
	{"char",	TCHAR	},
	{"div",		TDIV	},
	{"do",		TDO	},
	{"else",	TELSE	},
	{"end",		TEND	},
	{"false",	TFALSE	},
	{"if",		TIF	},
	{"integer",	TINTEGER},
	{"not",		TNOT	},
	{"of",		TOF	},
	{"or",		TOR	},
	{"procedure", TPROCEDURE},
	{"program",	TPROGRAM},
	{"read",	TREAD	},
	{"readln",	TREADLN },
	{"return", 	TRETURN },
	{"then",	TTHEN	},
	{"true",	TTRUE	},
	{"var",		TVAR	},
	{"while",	TWHILE	},
	{"write",	TWRITE  },
	{"writeln",	TWRITELN}
};

/* Token counter */
int numtoken[NUMOFTOKEN+1];

/* string of each token */
char *tokenstr[NUMOFTOKEN+1] = {
	"",
	"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
	 "else", "procedure", "return", "call", "while", "do", "not", "or", 
	"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
	 "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", 
	">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};

int token;

int main(int nc, char *np[]) {

  if(nc < 2) {
	  printf("File name id not given.\n");
	  return 0;
  }
  if(init_scan(np[1]) < 0) {
	  printf("File %s can not open.\n", np[1]);
	  return 0;
  }
  /* casl出力準備 */
  
  if(open_caslfile(np[1]) < 0){
    printf("Couldn't create casl program output file.\n");
    return 0;
  }
  /* 構文解析を行う */
  token = Scan();
  if(parse_program() == ERROR) return -1;
  end_scan();


  /* クロスリファレンサの出力*/
  
  print_cridloot();

  return 0;
}

int error(char *mes) {
	printf("\n ERROR: %s(line:%d)\n", mes,get_linenum());
	end_scan();
  return ERROR;
}
