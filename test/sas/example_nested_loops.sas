options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Nested DO Loops Example';

data mylib.out; 
    do i = 1 to 2;
        do j = 1 to 3;
            sum = i + j;
            output; 
        end;
    end;
run;

proc print data=mylib.out;
run;
