options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'ELSE IF Syntax Error Example';

data mylib.out; 
    set mylib.in; 
    if x > 10 then do;
        category = 'High';
        y = y * 2;
    else do; /* Missing 'end;' before 'else' */
        category = 'Low';
        y = y + 5;
    end;
    output; 
run;

proc print data=mylib.out;
run;
