options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Undefined Array Example';

data mylib.out; 
    set mylib.in; 
    do i = 1 to 3;
        temp{i} = temp{i} + 5; /* 'temp' array not defined */
    end;
    output; 
run;

proc print data=mylib.out;
run;
