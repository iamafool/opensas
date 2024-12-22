options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Function Evaluation Example';

data mylib.out; 
    set mylib.in; 
    sqrt_x = sqrt(x); 
    abs_diff = abs(y - 25); 
    log_y = log(y); 
    if sqrt_x > 3 and abs_diff < 10 then output; 
run;

proc print data=mylib.out;
run;