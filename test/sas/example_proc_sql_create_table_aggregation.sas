options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC SQL CREATE TABLE with Aggregation Example';

proc sql;
    create table mylib.department_summary as
    select department, count(*) as num_employees, avg(salary) as avg_salary
    from mylib.employees
    group by department;
quit;

proc print data=mylib.department_summary;
    run;
