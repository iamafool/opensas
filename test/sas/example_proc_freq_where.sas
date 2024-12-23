options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC FREQ with WHERE Condition Example';

proc freq data=mylib.sales_data;
    tables product;
    where region = 'West';
    run;

proc print data=mylib.sales_freq;
    run;
