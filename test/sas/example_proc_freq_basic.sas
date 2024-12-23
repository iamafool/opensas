options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic PROC FREQ Example';

proc freq data=mylib.customers;
    tables gender;
    run;

proc print data=mylib.customers_freq;
    run;
