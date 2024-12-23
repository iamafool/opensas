options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SQL SELECT with WHERE and ORDER BY Example';

proc sql;
    select name, revenue
    from mylib.sales
    where revenue >= 1500
    order by name;
quit;

proc print data=SQL_RESULT;
    run;
