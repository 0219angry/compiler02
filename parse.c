#include "token-list.h"
#include "parse.h"

extern int token;

int parse_program(){
  if(token != TPROGRAM) return(error("Keyword 'program' is not found"));
  token = Scan();
  if(token != TNAME) return(error("Promgram name is not found"));
  token = Scan();
  if(token != TSEMI) return(error("Semicolon is not found"));
  token = Scan();
  if(parse_block() == ERROR) return(ERROR);
  if(token != TDOT) return(error("Period is not found at the end of program"));
  token = Scan();
  return(NORMAL);
}

int parse_block(){
  while(token == TVAR || token == TPROCEDURE){
    if(token == TVAR){
      if(parse_variable_declaration() == ERROR) return(ERROR);
    }else if(token == TPROCEDURE){
      if(parse_subprogram_declaration() == ERROR) return(ERROR);
    }else{
      return(error("Block is not found"));
    }
  }
  if(parse_compound_statement() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_variable_declaration(){
  if(token != TVAR) return(error("Keyword 'var' is not found"));
  token = Scan();
  while(token == TNAME){
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    token = Scan();
    if(parse_type() == ERROR) return(ERROR);
    if(token != TSEMI) return(error("Semicolon is not found"));
    token = Scan();
  }
  return(NORMAL);
}

int parse_variable_names(){
  while(1){
    if(token != TNAME) return(error("variable name is not found"));
    token = Scan();
    if(token != TCOMMA) break;
    token = Scan();
  }
  return(NORMAL);
}

int parse_type(){
  if(parse_standard_type() == NORMAL){
    return(NORMAL);
  }else if(parse_array_type() == NORMAL){
    return(NORMAL);
  }else{
    return(ERROR);
  }
}

int parse_standard_type(){
  switch (token){
  case TINTEGER:
    token = Scan();
    return(NORMAL);
    break;
  case TBOOLEAN:
    token = Scan();
    return(NORMAL);
    break;
  case TCHAR:
    token = Scan();
    return(NORMAL);
    break;
  default:
    return(error("Standard type is not found"));
    break;
  }
}

int parse_array_type(){
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
  if(parse_standard_type() == ERROR) return(ERROR);
}

int parse_subprogram_declaration(){
  if(token !=TPROCEDURE) return(error("keyword 'procedure' is not found"));
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  if(token == TLPAREN){
    if(parse_formal_parameters() == ERROR) return(ERROR);
  }
  if(token != TSEMI) return(error("Semicolon is not found"));
  token = Scan();
  if(token == TVAR){
    if(parse_variable_declaration() == ERROR) return(ERROR);
  }
  if(parse_compound_statement() == ERROR) return(ERROR);
  if(token != TSEMI) return(error("Semicolon is not found"));
  token = Scan();
  return(NORMAL);
}

int parse_procedure_name(){
  if(token != TNAME) return(error("procedure name is not found"));
  token = Scan();
  return(NORMAL);
}

int parse_formal_parameters(){
  if(token != TLPAREN) return(error("Left parenthese is not found"));
  token = Scan();
  while(1){
    if(parse_variable_names() == ERROR) return(ERROR);
    if(token != TCOLON) return(error("Colon is not found"));
    token = Scan();
    if(parse_type() == ERROR) return(ERROR);
    if(token == TRPAREN) break;
    if(token != TSEMI) return(error("Semicolon is not found"));
  }
  token = Scan();
  return(NORMAL);
}

int parse_compound_statement(){
  if(token != TBEGIN) return(error("Keyword 'begin' is not found"));
  token = Scan();
  if(parse_statement() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_statement(){
}

int parse_condition_statement(){
  if(token != TIF) return(error("Keyword 'if' is not found"));
  token = Scan();
  if(parse_expression() == ERROR) return(ERROR);
  if(token != TTHEN) return(error("Keyword 'then' is not found"));
  token = Scan();
  if(parse_statement() == ERROR) return(ERROR);
  if(token == TELSE){
    token = Scan();
    if(parse_statement() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_iteration_statement(){
  if(token != TWHILE) return(error("Keyword 'while' is not found"));
  token = Scan();
  if(parse_expression() == ERROR) return(ERROR);
  if(token != TDO) return(error("Keyword 'do' is not found"));
  token = Scan();
  if(parse_statement() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_exit_statement(){
  if(token != TBREAK) return(error("Keyword 'break' is not found"));
  token = Scan();
  return(NORMAL);
}

int parse_call_statement(){
  if(token != TCALL) return(error("Keyword 'call' is not found"));
  token = Scan();
  if(parse_procedure_name() == ERROR) return(ERROR);
  if(token == TLPAREN){
    token = Scan();
    if(parse_expressions() == ERROR) return(ERROR);
    if(token != TRPAREN) return(error("Right parenthese is not found"));
    token = Scan();
  }
  return(NORMAL);
}

int parse_expressions(){
  while(token == TPLUS || token == TMINUS || token == TNAME 
    || token == TNUMBER || token == TTRUE || token == TFALSE 
    || token == TSTRING || token == TLPAREN || token == TNOT 
    || token == TINTEGER || token == TBOOLEAN || token == TCHAR){
      if(parse_expression() == ERROR) return(ERROR);
    }
    return(NORMAL);
}

int parse_return_statement(){
  if(token != TRETURN) return(error("Keyword 'return' is not found"));
  token = Scan();
  return(NORMAL);
}

int parse_assignment_statement(){
  if(parse_left_part() == ERROR) return(ERROR);
  if(token != TASSIGN) return(error("Assignment symbol is not found"));
}

int parse_left_part(){
  if(parse_variable() == ERROR) return(ERROR);
  return(NORMAL);
}

int parse_variable(){
  if(parse_variable_name() == ERROR) return(ERROR);
  if(token == TLSQPAREN){
    token = Scan();
    if(parse_expression() == ERROR) return(ERROR);
    if(token != TRSQPAREN) return(error("Right squere parenthese is not found"));
    token = Scan();
  }
  return(NORMAL);
}

int parse_expression(){
  if(parse_simple_expression() == ERROR) return(ERROR);
  while(token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ){
    if(parse_relational_operator() == ERROR) return(ERROR);
    if(parse_simple_expression() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_simple_expression(){
  if(token == TPLUS){
    token = Scan();
  }else if(token == TMINUS){
    token = Scan();
  }
  if(parse_term() == ERROR) return(ERROR);
  while(token == TPLUS || token == TMINUS || token == TOR){
    if(parse_additive_operator() == ERROR) return(ERROR);
    if(parse_term() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_term(){
  if(parse_factor() == ERROR) return(ERROR);
  while(token == TSTAR || token == TDIV || token == TAND){
    if(parse_multiplicative_operator() == ERROR) return(ERROR);
    if(parse_factor() == ERROR) return(ERROR);
  }
  return(NORMAL);
}

int parse_factor(){
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
      token = Scan();
      if(parse_expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right parenthese is not found"));
      token = Scan();
      break;
    case TNOT:
      token = Scan();
      if(parse_factor() == ERROR) return(ERROR);
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if(parse_standard_type() == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("Left paranthese is not found"));
      if(parse_expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error("Right paranthese is not found"));
      token = Scan();
      break;
    default:
      return(error("Factor is not found"));
  }
  return(NORMAL);
}

int parse_constant(){

}

int parse_multiplicative_operator(){
  switch(token){
    case TSTAR:
      token = Scan();
      return(NORMAL);
      break;
    case TDIV:
      token = Scan();
      return(NORMAL);
      break;
    case TAND:
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("Multiplicative operator is not found"));
  }
  return(NORMAL);
}

int parse_additive_operator(){
  switch(token){
    case TPLUS:
      token = Scan();
      return(NORMAL);
      break;
    case TMINUS:
      token = Scan();
      return(NORMAL);
      break;
    case TOR:
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("additive operator is not found"));
  }
  return(NORMAL);
}

int parse_relational_operator(){
  switch(token){
    case TEQUAL:
      token = Scan();
      return(NORMAL);
      break;
    case TNOTEQ:
      token = Scan();
      return(NORMAL);
      break;
    case TLE:
      token = Scan();
      return(NORMAL);
      break;
    case TLEEQ:
      token = Scan();
      return(NORMAL);
      break;
    case TGR:
      token = Scan();
      return(NORMAL);
      break;
    case TGREQ:
      token = Scan();
      return(NORMAL);
      break;
    default:
      return(error("Relational operator is not found"));
  }
  return(NORMAL);
}

int parse_input_statement(){
  if(token == TREAD){
    token = Scan();
  }else if(token == TREADLN){
    token = Scan();
  }else{
    return(error("Keyword 'read' or 'readln' is not found"));
  }
  if(token == TLPAREN){
    token == Scan();
    if(parse_variable() == ERROR) return(ERROR);
    while(token == TCOMMA){
      token = Scan();
      parse_variable();
    }
  }
  return(NORMAL);
}


int parse_output_statement(){
  if(token == TWRITE){
    token = Scan();
  }else if(token == TWRITELN){
    token = Scan();
  }else{
    return(error("Keyword 'write' or 'writeln' is not found"));
  }
  if(token == TLPAREN){
    token == Scan();
    if(parse_variable() == ERROR) return(ERROR);
    while(token == TCOMMA){
      token = Scan();
      parse_variable();
    }
  }
  return(NORMAL);
}

int parse_output_format(){
  if(token == TSTRING){
    token = Scan();
  }else{
    if(parse_expression() == ERROR) return(ERROR);
    if(token == TCOLON){
      token = Scan();
      if(token != TNUMBER) return(error("Unsigned integer is not found"));
      token = Scan();
    }
  }
  return(NORMAL);
}

int parse_empty_statement(){
  return(NORMAL);
}