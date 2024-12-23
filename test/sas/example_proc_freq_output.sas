options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC FREQ with OUTPUT Statement Example';

proc freq data=mylib.customers;
    tables gender / out=mylib.gender_freq;
    run;

proc print data=mylib.gender_freq;
    run;
