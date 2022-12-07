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

/* scan.c */
extern int token;
int indent_count;

/* semantic.c */
extern ID *tempID;
extern int isloot;



int parse_program(void){
  indent_count = 0;
  if(token != TPROGRAM) return(error("Keyword 'program' is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  if(token != TNAME) return(error("Promgram name is not found"));
  printf("%s", string_attr);
  token = Scan();
  if(token != TSEMI) return(error("Semicolon is not found"));
  printf("%s" ,token_str[token]);
  indent_count++;
  print_indent(indent_count);
  token = Scan();
  if(parse_block() == ERROR) return(ERROR);
  if(token != TDOT) return(error("Period is not found at the end of program"));
  printf("%s" ,token_str[token]);
  token = Scan();
  indent_count--;
  print_indent(indent_count);
  return(NORMAL);
}

int parse_block(void){
  while(token == TVAR || token == TPROCEDURE){
    if(token == TVAR){
      if(parse_variable_declaration() == ERROR) return(ERROR);
    }else if(token == TPROCEDURE){
      if(parse_subprogram_declaration() == ERROR) return(ERROR);
    }
    print_indent(indent_count);
  }
  indent_count--;
  print_indent(indent_count);
  if(parse_compound_statement() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_variable_declaration(void){
  if(token != TVAR) return(error("Keyword 'var' is not found"));
  printf("%s" ,token_str[token]);
  indent_count++;
  token = Scan();
  while(token == TNAME){
    print_indent(indent_count);
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    printf(" %s " ,token_str[token]);
    token = Scan();
    if(parse_type() == ERROR) return(ERROR);
    if(token != TSEMI) return(error("Semicolon is not found"));
    printf("%s" ,token_str[token]);
    token = Scan();
  }
  indent_count--;
  return(NORMAL);
}

int parse_variable_names(void){
  if(parse_variable_name() == ERROR) return(ERROR);
  while(token == TCOMMA){
    printf(" %s " ,token_str[token]);
    
    token = Scan();
    if(parse_variable_name() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_variable_name(void){
  if(token != TNAME) return(error("Variable name is not found"));
  printf("%s", string_attr);
  char *newname = malloc(sizeof(char) * MAXSTRSIZE);
  strcpy(newname, string_attr);
  //複数のvariable nameがあったときどう処理するか
  token = Scan();
  return(NORMAL);
}

int parse_type(void){
  switch(token){
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if(parse_standard_type() == ERROR) return(ERROR);
      break;
    case TARRAY:
      if(parse_array_type() == ERROR) return(ERROR);
      break;
    default:
      return(error("type is not found"));
  }
  return(NORMAL);
}

int parse_standard_type(void){
  switch (token){
  case TINTEGER:
    printf("%s" ,token_str[token]);
    token = Scan();
    return(NORMAL);
    break;
  case TBOOLEAN:
    printf("%s" ,token_str[token]);
    token = Scan();
    return(NORMAL);
    break;
  case TCHAR:
    printf("%s" ,token_str[token]);
    token = Scan();
    return(NORMAL);
    break;
  default:
    return(error("Standard type is not found"));
    break;
  }
}

int parse_array_type(void){
  if(token != TARRAY) return(error("Keyword 'array' is not found"));
  printf("%s" ,token_str[token]);
  token = Scan();
  if(token != TLSQPAREN) return(error("Left squere parenthese is not found"));
  printf("%s" ,token_str[token]);
  token = Scan();
  if(token != TNUMBER) return(error("Unsigned integer is not found"));
  printf("%d" ,num_attr);
  token = Scan();
  if(token != TRSQPAREN) return(error("Left squere parenthese is not found"));
  printf("%s " ,token_str[token]);
  token = Scan();
  if(token != TOF) return(error("Keyword 'of' is not found"));
  printf("%s " ,token_str[token]);
  token = Scan();
  if(parse_standard_type() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_subprogram_declaration(void){
  indent_count++;
  if(token !=TPROCEDURE) return(error("keyword 'procedure' is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  if(token == TLPAREN){
    if(parse_formal_parameters() == ERROR) return(ERROR);
  }
  if(token != TSEMI) return(error("Semicolon is not found"));
  printf("%s",token_str[token]);
  print_indent(indent_count);
  token = Scan();
  if(token == TVAR){
    if(parse_variable_declaration() == ERROR) return(ERROR);
  }
  if(parse_compound_statement() == ERROR) return(ERROR);
  if(token != TSEMI) return(error("Semicolon is not found"));
  printf("%s",token_str[token]);
  token = Scan();
  indent_count--;
  return(NORMAL);
}

int parse_procedure_name(void){
  if(token != TNAME) return(error("procedure name is not found"));
  printf("%s",string_attr);
  token = Scan();
  return(NORMAL);
}

int parse_formal_parameters(void){
  if(token != TLPAREN) return(error("Left parenthese is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  while(1){
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    printf(" %s ",token_str[token]);
    token = Scan();
    if(parse_type() == ERROR) return(ERROR);
    if(token == TRPAREN) break;
    if(token != TSEMI) return(error("Semicolon is not found"));
    printf("%s ",token_str[token]);
    token = Scan();
  }
  printf("%s",token_str[token]);
  token = Scan();
  return(NORMAL);
}

int parse_compound_statement(void){
  if(token != TBEGIN) return(error("Keyword 'begin' is not found"));
  printf("%s",token_str[token]);
  indent_count++;
  print_indent(indent_count);
  token = Scan();
  if(parse_statement() == ERROR) return(ERROR);
  while(token == TSEMI){
    printf("%s",token_str[token]);
    print_indent(indent_count);
    token = Scan();
    if(parse_statement() == ERROR) return(ERROR);
  }
  indent_count--;
  print_indent(indent_count);
  if(token != TEND) return(error("Keyword 'end' is not found"));
  printf("%s",token_str[token]);
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
  if(token != TIF) return(error("Keyword 'if' is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  if(parse_expression() == ERROR) return(ERROR);
  if(token != TTHEN) return(error("Keyword 'then' is not found"));
  printf(" %s",token_str[token]);
  token = Scan();
  indent_count++;
  print_indent(indent_count);
  if(parse_statement() == ERROR) return(ERROR);

  if(token == TELSE){
    indent_count--;
    print_indent(indent_count);
    printf("%s",token_str[token]);
    token = Scan();
    indent_count++;
    print_indent(indent_count);
    if(parse_statement() == ERROR) return(ERROR);  
  }
  indent_count--;
  return(NORMAL);
}

int parse_iteration_statement(void){
  if(token != TWHILE) return(error("Keyword 'while' is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  if(parse_expression() == ERROR) return(ERROR);
  if(token != TDO) return(error("Keyword 'do' is not found"));
  printf(" %s ",token_str[token]);
  token = Scan();
  indent_count++;
  print_indent(indent_count);
  if(parse_statement() == ERROR) return(ERROR);
  indent_count--;
  return(NORMAL);
}

int parse_exit_statement(void){
  if(token != TBREAK) return(error("Keyword 'break' is not found"));
  printf("%s",token_str[token]);
  token = Scan();
  return(NORMAL);
}

int parse_call_statement(void){
  if(token != TCALL) return(error("Keyword 'call' is not found"));
  printf("%s ",token_str[token]);
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  if(token == TLPAREN){
    printf("%s ",token_str[token]);
    token = Scan();
    if(parse_expressions() == ERROR) return(ERROR);
    if(token != TRPAREN) return(error("Right parenthese is not found"));
    printf(" %s",token_str[token]);
    token = Scan();
  }
  return(NORMAL);
}

int parse_expressions(void){
  if(parse_expression() == ERROR) return(ERROR);
  while(token == TCOMMA){
      printf(" %s ",token_str[token]);
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
    }
    return(NORMAL);
}

int parse_return_statement(void){
  if(token != TRETURN) return(error("Keyword 'return' is not found"));
  printf("%s",token_str[token]);
  token = Scan();
  return(NORMAL);
}

int parse_assignment_statement(void){
  if(parse_left_part() == ERROR) return(ERROR);
  if(token != TASSIGN) return(error("Assignment symbol is not found"));
  printf(" %s ",token_str[token]);
  token = Scan();
  if(parse_expression() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_left_part(void){
  if(parse_variable() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_variable(void){
  if(parse_variable_name() == ERROR) return(ERROR);
  if(token == TLSQPAREN){
    printf("%s ",token_str[token]);
    token = Scan();
    if(parse_expression() == ERROR) return(ERROR);
    if(token != TRSQPAREN) return(error("Right squere parenthese is not found"));
    printf("%s ",token_str[token]);
    token = Scan();
  }
  return(NORMAL);
}

int parse_expression(void){
  if(parse_simple_expression() == ERROR) return(ERROR);
  while(token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ){
    if(parse_relational_operator() == ERROR) return(ERROR);
    if(parse_simple_expression() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_simple_expression(void){
  if(token == TPLUS){
    printf("%s",token_str[token]);
    token = Scan();
  }else if(token == TMINUS){
    printf("%s",token_str[token]);
    token = Scan();
  }
  if(parse_term() == ERROR) return(ERROR);
  while(token == TPLUS || token == TMINUS || token == TOR){
    if(parse_additive_operator() == ERROR) return(ERROR);
    if(parse_term() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_term(void){
  if(parse_factor() == ERROR) return(ERROR);
  while(token == TSTAR || token == TDIV || token == TAND){
    if(parse_multiplicative_operator() == ERROR) return(ERROR);
    if(parse_factor() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_factor(void){
  switch(token){
    case TNAME:
      if(parse_variable() == ERROR) return(ERROR);
      break;
    case TNUMBER:
    case TTRUE:
    case TFALSE:
    case TSTRING:
      if(parse_constant() == ERROR) return(ERROR);
      break;
    case TLPAREN:
      printf("%s ",token_str[token]);
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right parenthese is not found"));
      printf(" %s",token_str[token]);
      token = Scan();
      break;
    case TNOT:
      printf("%s ",token_str[token]);
      token = Scan();
      if(parse_factor() == ERROR) return(ERROR);
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if(parse_standard_type() == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("Left paranthese is not found"));
      printf("%s ",token_str[token]);
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right paranthese is not found"));
      printf("%s ",token_str[token]);
      token = Scan();
      break;
    default:
      return(error("Factor is not found"));
  }
  return(NORMAL);
}

int parse_constant(void){
  switch(token){
    case TNUMBER:
      printf("%d", num_attr);
      token = Scan();
      break;
    case TFALSE:
      printf("%s", token_str[token]);
      token = Scan();
      break;
    case TTRUE:
      printf("%s", token_str[token]);
      token = Scan();
      break;
    case TSTRING:
      printf("\'%s\'", string_attr);
      token = Scan();
      break;
    default:
      return(error("Constant is not found"));
  }
  return(NORMAL);
}

int parse_multiplicative_operator(void){
  switch(token){
    case TSTAR:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TDIV:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TAND:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("Multiplicative operator is not found"));
  }
  return(NORMAL);
}

int parse_additive_operator(void){
  switch(token){
    case TPLUS:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TMINUS:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TOR:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("additive operator is not found"));
  }
  return(NORMAL);
}

int parse_relational_operator(void){
  switch(token){
    case TEQUAL:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TNOTEQ:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TLE:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TLEEQ:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TGR:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    case TGREQ:
      printf(" %s ",token_str[token]);
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("Relational operator is not found"));
  }
  return(NORMAL);
}

int parse_input_statement(void){
  if(token == TREAD){
    printf("%s ",token_str[token]);
    token = Scan();
  }else if(token == TREADLN){
    printf("%s ",token_str[token]);
    token = Scan();
  }else{
    return(error("Keyword 'read' or 'readln' is not found"));
  }
  if(token == TLPAREN){
    printf("%s ",token_str[token]);
    token = Scan();
    if(parse_variable() == ERROR) return(ERROR);
    while(token == TCOMMA){
      printf("%s ",token_str[token]);
      token = Scan();
      if(parse_variable() == ERROR) return(ERROR);
    }
    if(token != TRPAREN) return(error("Right parenthese is not found"));
    printf(" %s",token_str[token]);
    token = Scan();
  }
  return(NORMAL);
}


int parse_output_statement(void){
  if(token == TWRITE){
    printf("%s ",token_str[token]);
    token = Scan();
  }else if(token == TWRITELN){
    printf("%s ",token_str[token]);
    token = Scan();
  }else{
    return(error("Keyword 'write' or 'writeln' is not found"));
  }
  if(token == TLPAREN){
    printf("%s ",token_str[token]);
    token = Scan();
    if(parse_output_format() == ERROR) return(ERROR);
    while(token == TCOMMA){
      printf("%s ",token_str[token]);
      token = Scan();
      if(parse_output_format() == ERROR) return(ERROR);
    }
    if(token != TRPAREN) return(error("Right paranthese is not found"));
    printf(" %s",token_str[token]);
    token = Scan();
  }
  return(NORMAL);
}

int parse_output_format(void){
  if(token == TSTRING){
    if(parse_constant() == ERROR) return(ERROR);
  }else{
    if(parse_expression() == ERROR) return(ERROR);
    if(token == TCOLON){
      printf("%s ",token_str[token]);
      token = Scan();
      if(token != TNUMBER) return(error("Unsigned integer is not found"));
      printf("%d",num_attr);
      token = Scan();
    }
  }
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