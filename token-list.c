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

int main(int nc, char *np[]) {
	int token;
  char message[MAXSTRSIZE];

  if(nc < 2) {
	  printf("File name id not given.\n");
	  return 0;
  }
  if(init_scan(np[1]) < 0) {
	  printf("File %s can not open.\n", np[1]);
	  return 0;
  }
  /* 作成する部分：トークンカウント用の配列？を初期化する */
  for(int i=0;i<NUMOFTOKEN+1;i++){
    numtoken[i] = 0;
  }

  while((token = scan()) >= 0) {
    /* 作成する部分：トークンをカウントする */
	  numtoken[token]++;
    printf("Line %d : %s\n", get_linenum(),tokenstr[token]);
  }
  if(token == -1){
    sprintf(message,"Lexical Error in line %d\n",get_linenum());
    error(message);
    return -1;
  }
  end_scan();
    /* 作成する部分:カウントした結果を出力する */

  for(int i=1;i<NUMOFTOKEN;i++){
    if(numtoken[i]>0){
      printf("\"%s\"\t%d\n",tokenstr[i],numtoken[i]);
    }
  }

  return 0;
}

int error(char *mes) {
	printf("\n ERROR: %s\n", mes);
	end_scan();
  return ERROR;
}
