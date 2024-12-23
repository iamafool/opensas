options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SQL Basic SELECT Example';

proc sql;
    select id, name, revenue
    from mylib.sales;
quit;

proc print data=SQL_RESULT;
    run;
