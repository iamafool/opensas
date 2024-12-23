options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic PROC SORT Example';

proc sort data=mylib.unsorted out=mylib.sorted;
    by id;
run;

proc print data=mylib.sorted;
run;
