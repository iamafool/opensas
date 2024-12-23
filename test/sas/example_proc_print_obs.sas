options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC PRINT with OBS= Option Example';

proc print data=mylib.sorted;
    obs=2;
    run;
