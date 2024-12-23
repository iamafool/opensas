options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SQL SELECT with Alias Example';

proc sql;
    select id as EmployeeID, name as EmployeeName, revenue as SalesRevenue
    from mylib.sales;
quit;

proc print data=SQL_RESULT;
    run;
