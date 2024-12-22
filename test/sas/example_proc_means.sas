options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS Example';

data mylib.out; 
    set mylib.in; 
    x = x + 2; 
    y = y * 2; 
    output; 
run;

proc means data=mylib.out; 
    var x y; 
run;
