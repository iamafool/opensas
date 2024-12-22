options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SORT Example';

data mylib.unsorted; 
    set mylib.in; 
    x = x + 2; 
    y = y * 2; 
    output; 
run;

proc sort data=mylib.unsorted; 
    by y x; 
run;

proc print data=mylib.unsorted;
run;
