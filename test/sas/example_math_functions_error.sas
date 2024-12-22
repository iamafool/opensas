options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Mathematical Functions Error Example';

data mylib.out; 
    set mylib.in; 
    result1 = log10(-10); /* Invalid argument */
    result2 = round(3.1415); /* Missing decimal_places */
    result3 = round(3.1415, 2);
    output; 
run;

proc print data=mylib.out;
run;
