options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SORT with NODUPKEY Example';

proc sort data=mylib.unsorted out=mylib.sorted nodupkey;
    by id;
run;

proc print data=mylib.sorted;
run;