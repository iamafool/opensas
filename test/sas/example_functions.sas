title 'Function Evaluation Example';
libname mylib "c:\data";

data mylib.in;
    input x y;
    datalines;
4,20
16,30
9,15
25,40
;
run;

data mylib.out; 
    set mylib.in; 
    sqrt_x = sqrt(x); 
    abs_diff = abs(y - 25); 
    log_y = log(y); 
    if sqrt_x > 3 and abs_diff < 10 then output; 
run;

proc print data=mylib.out;
run;
