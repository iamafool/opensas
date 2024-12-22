options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Date and Time Functions Error Example';

data mylib.out; 
    set mylib.in; 
    result1 = intck('month', start_date); /* Missing end_date */
    result2 = intnx('year', start_date, 1, 'middle'); /* Unsupported alignment */
    result3 = intck('hour', start_date, end_date); /* Unsupported interval */
    output; 
run;

proc print data=mylib.out;
run;
