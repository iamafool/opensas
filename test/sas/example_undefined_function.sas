options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Undefined Function Example';

data mylib.out; 
    set mylib.in; 
    new_var = unknown_func(name);
    output; 
run;

proc print data=mylib.out;
run;
