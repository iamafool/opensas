options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Built-in Date and Time Functions Example';

data mylib.out; 
    set mylib.in; 
    current_date = today();
    days_between = intck('day', start_date, end_date);
    next_week = intnx('day', start_date, 7, 'beginning');
    output; 
run;

proc print data=mylib.out;
run;
