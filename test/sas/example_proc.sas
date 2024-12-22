options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC PRINT Example';

data mylib.out; 
    set mylib.in; 
    x = x + 2; 
    if x > 10 then output; 
run;

proc print data=mylib.out;
run;