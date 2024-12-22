options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DO Loop Example';

data mylib.out; 
    set mylib.in; 
    retain sum 0;
    array nums {3} num1 num2 num3;
    do i = 1 to 3;
        nums{i} = nums{i} + 10;
        sum = sum + nums{i};
    end;
    drop i;
    keep x sum num1 num2 num3;
    if sum > 25 then output; 
run;

proc print data=mylib.out;
run;