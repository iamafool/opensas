options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC FREQ with Multiple Tables and Options Example';

proc freq data=mylib.employees;
    tables department*gender / chisq nocum;
    tables education;
    run;

proc print data=mylib.employees_freq;
    run;
