/* stdin of this test contains:
   4 (almost) identical lines like "Pattern:Capture" but they have different
   UTF8 characteristics

   Pattern:Capture
   Patt\xffern:Capture
   Patt\xffern:Cap\xffture
   Pattern:Cap\xffture
*/

use IO;

{
  var f = openmem();

  var w = f.writer();
  w.write("Pattern:Capture\n");
  w.write(b"Patt\xffern:Capture\n");
  w.write(b"Patt\xffern:Cap\xffture\n");
  w.write(b"Pattern:Cap\xffture\n");
  w.close();

  // read from it -- following four reads are how these should be read
  var r = f.reader();
  var captureString: string;
  var captureBytes: bytes;

  writeln("Case 1");
  r.readf("%/Pattern:(.*)\n/", captureString);
  writeln("Captured string length should be 7 : ", captureString.length);

  writeln("Case 2");
  r.readf(b"%/Patt\xffern:(.*)\n/", captureString);
  writeln("Captured string length should be 7 : ", captureString.length);

  writeln("Case 3");
  r.readf(b"%/Patt\xffern:(.*)\n/", captureBytes);
  writeln("Captured bytes length should be 8 : ", captureBytes.length);

  writeln("Case 4");
  r.readf(b"%/Pattern:(.*)\n/", captureBytes);
  writeln("Captured bytes length should be 8 : ", captureBytes.length);

  r.close();
  f.close();
}


{
  var f = openmem();

  var w = f.writer();
  w.write("Pattern:Capture\n");
  w.write(b"Patt\xffern:Capture\n");
  w.write(b"Patt\xffern:Cap\xffture\n");
  w.write(b"Pattern:Cap\xffture\n");
  w.close();

  // read from it -- following reads are somewhat more interesting and some
  // should fail
  var r = f.reader();
  var captureString: string;
  var captureBytes: bytes;

  writeln("Case 5");
  // you can read the first line however you want, here test reading it with a
  // bytes pattern and storing it in a string  -- should work fine
  r.readf(b"%/Pattern:(.*)\n/", captureString);
  writeln("Captured string length should be 7 : ", captureString.length);

  writeln("Case 6");
  // you cannot create a string literal for this pattern anyway, so here try to
  // capture the match in a bytes -- should work fine
  r.readf(b"%/Patt\xffern:(.*)\n/", captureBytes);
  writeln("Captured string length should be 7 : ", captureBytes.length);

  writeln("Case 7 -- Take 1");
  // here the capture is non UTF8 -- so cannot be captured in a string
  try! {  // halt if we get a different error
    r.readf(b"%/Patt\xffern:(.*)\n/", captureString);
    writeln("(FAILURE) Captured string length : ", captureString.length);
  }
  catch e: SystemError {
    writeln("Caught expected error: ");
    writeln(e);
  }

  writeln("Case 7 -- Take 2");
  // here the channel must have been rewound back. Read it the right way to move
  // it forward
  r.readf(b"%/Patt\xffern:(.*)\n/", captureBytes);
  writeln("Captured bytes length should be 8 : ", captureBytes.length);

  writeln("Case 8 -- Take 1");
  // Case 4 above can go wrong in two ways:
  // (1) use a string regular expression to capture non UTF8 data -- should be
  // BadFormatError
  try! {
    r.readf("%/Pattern:(.*)\n/", captureBytes);
    writeln("(FAILURE) Captured bytes length : ", captureBytes.length);
  }
  catch e: BadFormatError {
    writeln("Caught expected error");
    writeln(e);
  }

  writeln("Case 8 -- Take 2");
  // (2) correctly use a bytes regular expression but try to capture the match
  // into a string -- should be argument mismatch
  try! {
    r.readf(b"%/Pattern:(.*)\n/", captureString);
    writeln("(FAILURE) Captured string length : ", captureString.length);
  }
  catch e: SystemError {
    writeln("Caught expected error");
    writeln(e);
  }

  writeln("Case 8 -- Take 3");
  // read it correctly just for the sake of it
  r.readf(b"%/Pattern:(.*)\n/", captureBytes);
  writeln("Captured bytes length should be 8 : ", captureBytes.length);

  r.close();
  f.close();
}

