var x: uint = 8u;

config var testit = true;

var y = if (testit) then x else 0u;

writeln("y is: ", y);


y = if (testit) then x else 0;

writeln("y is: ", y);


y = if (testit) then 0 else x;

writeln("y is: ", y);

