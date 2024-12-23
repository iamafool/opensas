options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC PRINT with VAR Statement Example';

proc print data=mylib.sorted;
    var id name;
    run;
