options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC FREQ Cross-Tabulation with Chi-Square Example';

proc freq data=mylib.sales_data;
    tables gender*purchase / chisq;
    run;

proc print data=mylib.sales_freq;
    run;
