options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SQL CREATE TABLE Example';

proc sql;
    create table mylib.top_sales as
    select id, name, revenue
    from mylib.sales
    where revenue > 1500
    order by revenue desc;
quit;

proc print data=mylib.top_sales;
    run;
