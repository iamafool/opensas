options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SORT with WHERE Condition Example';

proc sort data=mylib.unsorted out=mylib.sorted;
    by name;
    where id > 1;
run;

proc print data=mylib.sorted;
run;
