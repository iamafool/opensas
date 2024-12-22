options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Array-Based Conditional Operations Example';

data mylib.out; 
    set mylib.in; 
    array temp{4} temp1 temp2 temp3 temp4;
    do i = 1 to 4;
        if temp{i} > 50 then temp{i} = 50;
    end;
    output; 
run;

proc print data=mylib.out;
run;
