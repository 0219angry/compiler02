program test;
/* 宣言 */
var i,j : integer;
  a : char;
  b : boolean;
  arrayi : array[10] of integer;
  arraya : array[10] of char;
  arrayb : array[10] of boolean;

/* 関数宣言 */
procedure keisan(n,m : integer;x : integer);
begin
  if x = 1 then
    n := m + 2
  else if x = 2 then
    n := m - 2
  else if x = 3 then
    n := m * 2
  else if x = 4 then
    n := m div 2;
  writeln(n)
end;

begin
  i := 10;
  j := 10;
  writeln('i=',i,',j=',j);
  call keisan(i,j,1);
  writeln('Add',i);
  call keisan(i,j,2);
  writeln('Sub',i);
  call keisan(i,j,3);
  writeln('Mul',i);
  call keisan(i,j,4);
  writeln('Div',i);
end.