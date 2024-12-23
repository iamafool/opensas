options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC PRINT with Multiple Options Example';

proc print data=mylib.sorted;
    var id name;
    obs=3;
    noobs;
    label;
    run;
