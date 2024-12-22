options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Error Handling Example';

data mylib.out; 
    set mylib.in; 
    x = ; /* Missing expression */
    if x then output; 
run;

data mylib.out2;
    set mylib.undefined; /* Undefined dataset */
    y = y + 5;
    output;
run;

proc print data=mylib.out;
run;