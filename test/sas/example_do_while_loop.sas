options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DO WHILE Loop Example';

data mylib.out; 
    i = 1;
    do while (i <= 3);
        square = i * i;
        output; 
        i + 1;
    end;
run;

proc print data=mylib.out;
run;
