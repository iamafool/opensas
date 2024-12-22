options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Built-in String Functions Example';

data mylib.out; 
    set mylib.in; 
    first_part = substr(name, 1, 3);
    trimmed = trim(name);
    upper_name = upcase(name);
    lower_name = lowcase(name);
    output; 
run;

proc print data=mylib.out;
run;
