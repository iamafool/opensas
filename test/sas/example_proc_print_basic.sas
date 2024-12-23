options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic PROC PRINT Example';

proc print data=mylib.sorted;
    run;
