options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'ELSE IF Block Syntax Error Example';

data mylib.out; 
    set mylib.in; 
    if x > 10 then do;
        category = 'High';
        y = y * 2;
    else if x > 5 then do; /* Missing 'end;' before 'else if' */
        category = 'Medium';
        y = y + 10;
    end;
    output; 
run;

proc print data=mylib.out;
run;
