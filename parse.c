#include "token-list.h"
#include "parse.h"

char *token_str[NUMOFTOKEN+1] = {
	"",
	"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
	 "else", "procedure", "return", "call", "while", "do", "not", "or", 
	"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
	 "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", 
	">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};

int indent_count = 0;

/* scan.c */
extern int token;
extern int line;
extern int current_line;

/* semantic.c */
extern ID *tempID; /* in variable declaration now defined IDs is here */
extern char *procname_attr; /* if global NULL, else procname pointer*/
extern ID *globalidloot;
extern ID *localidloot;
extern ID *procid;
extern PROC *cridloot;

/* caslmacro.c */
extern FILE * caslfilep;
extern int label;



/* process state */
int is_variable_declaration = 0; /* 0:out declaration 1:in declaration */
int is_in_procedure = 0; /* 0:out procedure 1:in procedure 2:procedure name*/
int is_call_statement = 0; /* 0:not call 1:call */
int is_formal_parameter = 0; /* 0:no 1:yes */

int typenum = 0; /* type number */
int arraysize = 0; /* if type array, size of array */

ID *referenced_val;

int if_true_label = 0;
int if_false_label = 0;
int iteration_label = 0;


int parse_program(void){
  indent_count = 0;
  if(token != TPROGRAM) return(error("Keyword 'program' is not found"));
  token = Scan();
  if(token != TNAME) return(error("Promgram name is not found"));
  if(asm_start(string_attr) == ERROR) return(ERROR);
  token = Scan();
  if(token != TSEMI) return(error("Semicolon is not found"));
  indent_count++;
  token = Scan();
  if(parse_block() == ERROR) return(ERROR);
  if(token != TDOT) return(error("Period is not found at the end of program"));
  token = Scan();
  indent_count--;
  add_utils();
  
  return(NORMAL);
}

int parse_block(void){
  while(token == TVAR || token == TPROCEDURE){
    if(token == TVAR){
      if(parse_variable_declaration() == ERROR) return(ERROR);
    }else if(token == TPROCEDURE){
      if(parse_subprogram_declaration() == ERROR) return(ERROR);
    }
    
    
  }
  indent_count--;
  regist_proc_global();

  
  if(parse_compound_statement() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_variable_declaration(void){
  is_variable_declaration = 1;
  if(token != TVAR) return(error("Keyword 'var' is not found"));
  
  indent_count++;
  token = Scan();
  while(token == TNAME){
    
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    token = Scan();
    int tp;
    if((tp = parse_type()) == ERROR) return(ERROR);
    TYPE *temptype;
    temptype = create_type(tp);
    add_type(temptype);
    regist_define();
    
    if(token != TSEMI) return(error("Semicolon is not found"));
    
    token = Scan();
  }
  indent_count--;

  is_variable_declaration = 0;
  return(NORMAL);
}

int parse_variable_names(void){
  if(parse_variable_name() == ERROR) return(ERROR);
  while(token == TCOMMA){
    token = Scan();
    if(parse_variable_name() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_variable_name(void){
  int ttype = NORMAL;
  if(token != TNAME) return(error("Variable name is not found"));
  if(is_variable_declaration == 1 || is_formal_parameter == 1){
    /* 変数宣言のとき */
    add_define_without_type();
    if(check_overloading(string_attr) == ERROR) return(ERROR);
  }else{
    /* 変数参照のとき */
    ID * id;
    if((id = add_reference(string_attr,line)) == NULL) return(ERROR);
    referenced_val = id; 
    ttype = (id->itp)->ttype;
  }
  token = Scan();
  return(ttype);
}

int parse_type(void){
  int tp;
  switch(token){
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if((tp = parse_standard_type()) == ERROR) return(ERROR);
      break;
    case TARRAY:
      if((tp = parse_array_type()) == ERROR) return(ERROR);
      break;
    default:
      return(error("type is not found"));
  }
  return(tp);
}

int parse_standard_type(void){
  switch (token){
  case TINTEGER:
    token = Scan();
    return(TPINT);
    break;
  case TBOOLEAN:
    token = Scan();
    return(TPBOOL);
    break;
  case TCHAR:
    token = Scan();
    return(TPCHAR);
    break;
  default:
    return(error("Standard type is not found"));
    break;
  }
}

int parse_array_type(void){
  int tp;
  if(token != TARRAY) return(error("Keyword 'array' is not found"));
  token = Scan();
  if(token != TLSQPAREN) return(error("Left squere parenthese is not found"));
  token = Scan();
  if(token != TNUMBER) return(error("Unsigned integer is not found"));
  token = Scan();
  if(token != TRSQPAREN) return(error("Left squere parenthese is not found"));
  token = Scan();
  if(token != TOF) return(error("Keyword 'of' is not found"));
  token = Scan();
  if((tp = parse_standard_type()) == ERROR) return(ERROR);
  switch(tp){
    case TPINT:
      tp = TPARRAYINT;
      break;
    case TPBOOL:
      tp = TPARRAYBOOL;
      break;
    case TPCHAR:
      tp = TPARRAYCHAR;
      break;
    default:
      return(ERROR);
  }
  return(tp);
}

int parse_subprogram_declaration(void){
  indent_count++;
  if(token !=TPROCEDURE) return(error("keyword 'procedure' is not found"));
  is_in_procedure = 2;
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  procid = tempID;
  regist_define();

  is_in_procedure = 1;
  if(token == TLPAREN){
    is_formal_parameter = 1;
    if(parse_formal_parameters() == ERROR) return(ERROR);
    is_formal_parameter = 0;
  }
  if(token != TSEMI) return(error("Semicolon is not found"));
  if(asm_proc_val(localidloot) == ERROR) return(ERROR);
  token = Scan();
  is_variable_declaration = 1;
  if(token == TVAR){
    if(parse_variable_declaration() == ERROR) return(ERROR);
  }
  is_variable_declaration = 0;
  if(asm_proc_start(localidloot,procname_attr) == ERROR) return(ERROR);
  if(parse_compound_statement() == ERROR) return(ERROR);
  if(token != TSEMI) return(error("Semicolon is not found"));
  token = Scan();
  indent_count--;
  // cridlootへの追加
  if(localidloot != NULL){
    regist_proc_local();
  }
  procname_attr = NULL;
  is_in_procedure = 0;
  return(NORMAL);
}

int parse_procedure_name(void){
  if(token != TNAME) return(error("procedure name is not found"));
  if(is_in_procedure == 2){
    /* サブプログラム宣言のとき */
    add_define_without_type();
    if(check_overloading(string_attr) == ERROR) return(ERROR);
    TYPE *temptype;
    temptype = create_type(TPPROC);
    add_type(temptype);
  }else{
    /* サブプログラム参照のとき */
    if(add_reference(string_attr,line) == NULL) return(ERROR);
  }
  token = Scan();
  return(NORMAL);
}

int parse_formal_parameters(void){
  if(token != TLPAREN) return(error("Left parenthese is not found"));
  
  token = Scan();
  while(1){
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    
    token = Scan();
    int ty;
    if((ty = parse_type()) == ERROR) return(ERROR);
    if(ty == TPARRAYINT || ty == TPARRAYCHAR || ty == TPARRAYBOOL){
      return(error("formal parameter cant take an array argument"));
      
    }
    // 関数引数の追加(対procedure)
    add_formal_type(procid->itp,ty);
    // 関数引数を変数として追加
    TYPE *temptype;
    temptype = create_type(ty);
    add_type(temptype);
    regist_define();
    
    if(token == TRPAREN) break;
    if(token != TSEMI) return(error("Semicolon is not found"));
    
    token = Scan();
  }
  
  token = Scan();
  return(NORMAL);
}

int parse_compound_statement(void){
  if(token != TBEGIN) return(error("Keyword 'begin' is not found"));
  
  indent_count++;
  
  token = Scan();
  if(parse_statement() == ERROR) return(ERROR);
  while(token == TSEMI){
    
    
    token = Scan();
    if(parse_statement() == ERROR) return(ERROR);
  }
  indent_count--;
  
  if(token != TEND) return(error("Keyword 'end' is not found"));
  
  token = Scan();
  return(NORMAL);
}

int parse_statement(void){
  switch(token){
    case TNAME:
      if(parse_assignment_statement() == ERROR) return(ERROR);
      break;
    case TIF:
      if(parse_condition_statement() == ERROR) return(ERROR);
      break;
    case TWHILE:
      if(parse_iteration_statement() == ERROR) return(ERROR);
      break;
    case TBREAK:
      if(parse_exit_statement() == ERROR) return(ERROR);
      break;
    case TCALL:
      if(parse_call_statement() == ERROR) return(ERROR);
      break;
    case TRETURN:
      if(parse_return_statement() == ERROR) return(ERROR);
      break;
    case TREAD:
    case TREADLN:
      if(parse_input_statement() == ERROR) return(ERROR);
      break;
    case TWRITE:
    case TWRITELN:
      if(parse_output_statement() == ERROR) return(ERROR);
      break;
    case TBEGIN:
      if(parse_compound_statement() == ERROR) return(ERROR);
      break;
    default:
      if(parse_empty_statement() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_condition_statement(void){
  int ttype;
  if(token != TIF) return(error("Keyword 'if' is not found"));
  
  token = Scan();
  if((ttype = parse_expression()) == ERROR) return(ERROR);
  if(ttype != TPBOOL){
    return(error("in condition statemente, expression needs return boolean"));
  }
  if(token != TTHEN) return(error("Keyword 'then' is not found"));
  
  token = Scan();
  indent_count++;
  
  if(parse_statement() == ERROR) return(ERROR);

  if(token == TELSE){
    indent_count--;
    
    
    token = Scan();
    indent_count++;
    
    if(parse_statement() == ERROR) return(ERROR);  
  }
  indent_count--;
  return(NORMAL);
}

int parse_iteration_statement(void){
  int ttype;
  if(token != TWHILE) return(error("Keyword 'while' is not found"));
  
  token = Scan();
  if((ttype = parse_expression()) == ERROR) return(ERROR);
  if(ttype != TPBOOL){
    return(error("in iteration statemente, expression needs return boolean"));
  }
  if(token != TDO) return(error("Keyword 'do' is not found"));
  
  token = Scan();
  indent_count++;
  
  if(parse_statement() == ERROR) return(ERROR);
  indent_count--;
  return(NORMAL);
}

int parse_exit_statement(void){
  if(token != TBREAK) return(error("Keyword 'break' is not found"));
  
  token = Scan();
  return(NORMAL);
}

int parse_call_statement(void){
  ID * called_proc;
  if(token != TCALL) return(error("Keyword 'call' is not found"));
  
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  called_proc = referenced_val;
  if(token == TLPAREN){
    
    token = Scan();
    if(parse_expressions() == ERROR) return(ERROR);
    if(token != TRPAREN) return(error("Right parenthese is not found"));
    
    token = Scan();
  }
  asm_call(called_proc);
  return(NORMAL);
}

int parse_expressions(void){
  if(parse_expression() == ERROR) return(ERROR);
  while(token == TCOMMA){
      
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
    }
    return(NORMAL);
}

int parse_return_statement(void){
  if(token != TRETURN) return(error("Keyword 'return' is not found"));
  
  token = Scan();
  return(NORMAL);
}

int parse_assignment_statement(void){
  int Ltype;
  int Rtype;
  if((Ltype = parse_left_part()) == ERROR) return(ERROR);
  if(token != TASSIGN) return(error("Assignment symbol is not found"));
  
  token = Scan();
  if((Rtype = parse_expression()) == ERROR) return(ERROR);
  if(Ltype != Rtype){
    return(error("in assignment statement, both side needs same type"));
  }
  asm_assign();
  return(NORMAL);
}

int parse_left_part(void){
  int ttype;
  if((ttype = parse_variable(1)) == ERROR) return(ERROR);
  return(ttype);
}

int parse_variable(int isleft){
  int ttype = NORMAL;
  int etype;
  ID * variable_part = referenced_val;
  if((ttype = parse_variable_name()) == ERROR) return(ERROR);
  if(token == TLSQPAREN){
    if(ttype != TPARRAY){
      return(error("variable with expression needs type array"));
    }
    
    token = Scan();
    if((etype = parse_expression()) == ERROR) return(ERROR); // expressionの値がstacktopに入っている
    if(etype != TPINT){
      return(error("array number needs type integer"));
    }
    if(token != TRSQPAREN) return(error("Right squere parenthese is not found"));

    if(isleft == 1){
      if(asm_ref_lval(variable_part) == ERROR) return(ERROR);
    }else{
      if(asm_ref_rval(variable_part) == ERROR) return(ERROR);
    }
    token = Scan();
  }
  return(ttype);
}

int parse_expression(void){
  int Ltype = NORMAL;
  int ope;
  int Rtype = NORMAL;
  if((Ltype = parse_simple_expression()) == ERROR) return(ERROR);
  while(token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ){
    if((ope = parse_relational_operator()) == ERROR) return(ERROR);
    if((Rtype = parse_simple_expression()) == ERROR) return(ERROR);
     if(Ltype != Rtype){
          return(error("relational operetor needs same type on both side"));
    }
    asm_expression(ope);
    Ltype = TPBOOL;
  }
  return(Ltype);
}

int parse_simple_expression(void){
  int isplusminus = 0;
  int Ltype = NORMAL;
  int ope;
  int Rtype = NORMAL;
  if(token == TPLUS){
    isplusminus = 1;
    
    token = Scan();
  }else if(token == TMINUS){
    isplusminus = -1;
    
    token = Scan();
  }
  if((Ltype = parse_term()) == ERROR) return(ERROR);
  if(isplusminus != 0){
    if(Ltype != TPINT){
      return(error("simple expression needs integer"));
    }
    if(isplusminus == -1){
      asm_minus_sign();
    }
  }
  while(token == TPLUS || token == TMINUS || token == TOR){
    if((ope = parse_additive_operator()) == ERROR) return(ERROR);
    if((Rtype = parse_term()) == ERROR) return(ERROR);
    switch(ope){
      case TMINUS:
      case TPLUS:
        if(Ltype != TPINT || Rtype != TPINT){
          return(error("additive operator + or - need integer"));
        }
        Ltype = TPINT;
        break;
      case TOR:
        if(Ltype != TPBOOL || Rtype != TPBOOL){
          return(error("additive operator or need integer"));
        }
        Ltype = TPBOOL;
        break;
    }
    switch(ope){
      case TPLUS:
        asm_ADDA();
        break;
      case TMINUS:
        asm_SUBA();
        break;
      case TOR:
        asm_OR();
        break;
    }
  }
  return(Ltype);
}

int parse_term(void){
  int Ltype = NORMAL;
  int ope;
  int Rtype = NORMAL;
  if((Ltype = parse_factor()) == ERROR) return(ERROR);
  while(token == TSTAR || token == TDIV || token == TAND){
    if((ope = parse_multiplicative_operator()) == ERROR) return(ERROR);
    if((Rtype = parse_factor()) == ERROR) return(ERROR);
    switch(ope){
      case TSTAR:
      case TDIV:
        if(Ltype != TPINT || Rtype != TPINT){
          return(error("multiplicative operator * or div need integer"));
        }
        Ltype = TPINT;
        break;
      case TAND:
        if(Ltype != TPBOOL || Rtype != TPBOOL){
          return(error("multiplicative operator and need integer"));
        }
        Ltype = TPBOOL;
        break;
    }
    switch(ope){
      case TSTAR:
        asm_MULA();
        break;
      case TDIV:
        asm_DIVA();
        break;
      case TAND:
        asm_AND();
        break;
    }
  }
  return(Ltype);
}

int parse_factor(void){
  int ttype = NORMAL;
  switch(token){
    case TNAME:
      if((ttype = parse_variable(0)) == ERROR) return(ERROR);
      break;
    case TNUMBER:
    case TTRUE:
    case TFALSE:
    case TSTRING:
      if((ttype = parse_constant()) == ERROR) return(ERROR);
      break;
    case TLPAREN:
      
      token = Scan();
      if((ttype = parse_expression()) == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right parenthese is not found"));
      
      token = Scan();
      break;
    case TNOT:
      
      token = Scan();
      if((ttype = parse_factor()) == ERROR) return(ERROR);
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if((ttype = parse_standard_type()) == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("Left paranthese is not found"));
      
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right paranthese is not found"));
      
      token = Scan();
      break;
    default:
      return(error("Factor is not found"));
  }
  return(ttype);
}

int parse_constant(void){
  int ttype = NORMAL;
  switch(token){
    case TNUMBER:
      ttype = TPINT;
      token = Scan();
      break;
    case TFALSE:
      ttype = TPBOOL;
      
      token = Scan();
      break;
    case TTRUE:
      ttype = TPBOOL;
      
      token = Scan();
      break;
    case TSTRING:
      if(strlen(string_attr) == 1){
        ttype = TPCHAR;
      }
      token = Scan();
      break;
    default:
      return(error("Constant is not found"));
  }
  return(ttype);
}

int parse_multiplicative_operator(void){
  switch(token){
    case TSTAR:
      
      token = Scan();
      return(TSTAR);
      break;
    case TDIV:
      
      token = Scan();
      return(TDIV);
      break;
    case TAND:
      
      token = Scan();
      return(TAND);
      break;
    default:
      return(error("Multiplicative operator is not found"));
  }
  return(NORMAL);
}

int parse_additive_operator(void){
  switch(token){
    case TPLUS:
      
      token = Scan();
      return(TPLUS);
      break;
    case TMINUS:
      
      token = Scan();
      return(TMINUS);
      break;
    case TOR:
      
      token = Scan();
      return(TOR);
      break;
    default:
      return(error("additive operator is not found"));
  }
  return(NORMAL);
}

int parse_relational_operator(void){
  switch(token){
    case TEQUAL:
      
      token = Scan();
      return(TEQUAL);
      break;
    case TNOTEQ:
      
      token = Scan();
      return(TNOTEQ);
      break;
    case TLE:
      
      token = Scan();
      return(TLE);
      break;
    case TLEEQ:
      
      token = Scan();
      return(TLEEQ);
      break;
    case TGR:
      
      token = Scan();
      return(TGR);
      break;
    case TGREQ:
      
      token = Scan();
      return(TGREQ);
      break;
    default:
      return(error("Relational operator is not found"));
  }
  return(NORMAL);
}

int parse_input_statement(void){
  int ttype;
  int ln;
  if(token == TREAD){
    ln = 0;
    token = Scan();
  }else if(token == TREADLN){
    ln = 1;
    token = Scan();
  }else{
    return(error("Keyword 'read' or 'readln' is not found"));
  }
  if(token == TLPAREN){
    
    token = Scan();
    if((ttype = parse_variable(0)) == ERROR) return(ERROR);
    if(ttype != TPCHAR && ttype != TPINT){
      return(error("read or readln needs integer or char "));
    }
    if(asm_read(referenced_val) == ERROR) return(ERROR);
    if(ln == 1){
      asm_readln();
    }
    while(token == TCOMMA){
      token = Scan();
      if((ttype = parse_variable(0)) == ERROR) return(ERROR);
      if(ttype != TPCHAR && ttype != TPINT){
        return(error("read or readln needs integer or char "));
      }
    }
    if(token != TRPAREN) return(error("Right parenthese is not found"));
    
    token = Scan();
  }
  return(NORMAL);
}


int parse_output_statement(void){
  int ln;
  if(token == TWRITE){
    ln = 0;
    token = Scan();
  }else if(token == TWRITELN){
    ln = 1;
    token = Scan();
  }else{
    return(error("Keyword 'write' or 'writeln' is not found"));
  }
  if(token == TLPAREN){
    
    token = Scan();
    if(parse_output_format() == ERROR) return(ERROR);
    if(ln == 1){
      asm_writeln();
    }
    while(token == TCOMMA){
      
      token = Scan();
      if(parse_output_format() == ERROR) return(ERROR);
      if(ln == 1){
        asm_writeln();
      }
    }
    if(token != TRPAREN) return(error("Right paranthese is not found"));
    
    token = Scan();
  }
  return(NORMAL);
}

int parse_output_format(void){
  int etype;
  int output_count = 0;
  if(token == TSTRING){
    if(parse_constant() == ERROR) return(ERROR);
    etype = TPSTR;
  }else{
    if((etype = parse_expression()) == ERROR) return(ERROR);
    if(etype != TPINT && etype != TPCHAR && etype != TPBOOL){
      return(error("output format needs standard type"));
    }
    if(token == TCOLON){
      
      token = Scan();
      if(token != TNUMBER) return(error("Unsigned integer is not found"));
      output_count = num_attr;
      token = Scan();
    }
  }
  asm_output_format(etype,output_count);
  return(NORMAL);
}

int parse_empty_statement(void){
  return(NORMAL);
}

void print_indent(int ic){
  
  printf("\n");
  for(int i=0;i<ic;i++){
    
    printf("    ");
  }
}