options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC PRINT with NOOBS Option Example';

proc print data=mylib.sorted;
    noobs;
    run;
