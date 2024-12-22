options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Built-in Mathematical Functions Example';

data mylib.out; 
    set mylib.in; 
    absolute = abs(x);
    ceiling = ceil(x);
    flooring = floor(x);
    rounded = round(x, 0.1);
    exponential = exp(x);
    logarithm = log10(x);
    output; 
run;

proc print data=mylib.out;
run;
