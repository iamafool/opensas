options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Simple DO Loop Example';

data mylib.out; 
    do i = 1 to 5;
        square = i * i;
        output; 
    end;
run;

proc print data=mylib.out;
run;
