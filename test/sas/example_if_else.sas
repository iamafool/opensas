options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'IF-ELSE Example';

data mylib.out; 
    set mylib.in; 
    if x > 10 then do;
        status = 'High';
        y = y * 2;
    end;
    else do;
        status = 'Low';
        y = y + 5;
    end;
    output; 
run;

proc print data=mylib.out;
run;
