program test;
/* 宣言 */
var i,j : integer;
  a : char;
  b : boolean;
  arrayi : array[5] of integer;
  arraya : array[10] of char;
  arrayb : array[8] of boolean;

/* 関数宣言 */
procedure keisan(n,m : integer;x : integer);
begin
  /* if文・四則演算 */
  if x = 1 then
    n := m + 2
  else if x = 2 then
    n := m - 2
  else if x = 3 then
    n := m * 2
  else if x = 4 then
    n := m div 2;
end;

procedure print(num : array[5] of integer);
var i : integer;
begin
  i := 4;
  while i>=0 do begin
    write('[',4-i,']',num[4-i],' ');
    i := i-1;
  end;
end;

begin
  write('Please enter number =');
  readln(j);
  write('Please enter character =');
  readln(a);
  writeln('input character is ',a);
  writeln('i=',i,',j=',j);
  call keisan(i,j,1);
  writeln('Add',i);
  call keisan(i,j,2);
  writeln('Sub',i);
  call keisan(i,j,3);
  writeln('Mul',i);
  call keisan(i,j,4);
  writeln('Div',i);
  writeln('Minus', -i);

  i := 26;
  a := '{';
  /* while文 */
  while i > 0 do begin
    write(char(integer(a)-i),' ');
    i := i-1;
  end;
  writeln; 
  /* array型 */
  arrayi[0] := 0;
  arrayi[1] := 1;
  arrayi[2] := 4;
  arrayi[3] := 9;
  arrayi[4] := 16;

  call print(arrayi);
  writeln;
  /* cast */
  writeln(integer(40),' | ',char(40),' | ',boolean(40));
  writeln(integer('a'),' | ',char('a'),' | ',boolean('a'));
  writeln(integer(true),' | ',char(true),' | ',boolean(true));

  arrayb[0] := true and true;
  arrayb[1] := true and false;
  arrayb[2] := false and true;
  arrayb[3] := false and false;

  arrayb[4] := true or true;
  arrayb[5] := true or false;
  arrayb[6] := false or true;
  arrayb[7] := false or false;

  writeln('and|true |false');
  writeln('T  |',arrayb[0],'|',arrayb[1]);
  writeln('F  |',arrayb[2],'|',arrayb[3]);

  writeln('or|true |false');
  writeln('T |',arrayb[4],'|',arrayb[5]);
  writeln('F |',arrayb[6],'|',arrayb[7]);
  
  /* 関係演算子 */
  writeln('10 < 1 is ',10 < 1);
  writeln('10 <= 1 is ',10 <= 1);
  writeln('10 = 1 is ',10 = 1);
  writeln('10 >= 1 is ',10 >= 1);
  writeln('10 > 1 is ',10 > 1);
  writeln('10 <> 1 is ',10 <> 1);
  writeln;
  writeln('1 < 1 is ',1 < 1);
  writeln('1 <= 1 is ',1 <= 1);
  writeln('1 = 1 is ',1 = 1);
  writeln('1 >= 1 is ',1 >= 1);
  writeln('1 <> 1 is ',1 <> 1);

  /* not因子 */
  b := true;
  writeln('b=',b);
  writeln('not b=',not b);
  writeln;
  i := 254;
  writeln('i=',i);
  writeln('not i=',not i);
end.