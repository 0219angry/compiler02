program parsetest;
  var a,b : integer;
  c : boolean;
  d : char;
  e : array[5] of char;

  procedure procedure1(a1, a2 : integer;b1, b2 : char);
    var a : integer;
    begin
    end;
  
  begin
    if a = b then
    a := b
    else if a <> b then
    a := b * c
    else if a < b then
    a := b div c
    else if a <= b then
    a := b and c
    else if a > b then
    a := -b + c
    else if a >= b then
    a := b - (a+b);
    while not a do
      begin
        a := b or c;
        break
      end;
    call procedure1(a+b,a-b);
    n1 := 1;
    n2 := false;
    n3 := true;
    n4 := 'string';
    n5 := char(a);
    read(a,a[a+b]);
    readln(a,b);
    write(a:3,'string' ;
    writeln(a);
    return;
    
  end.