options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DO Loop Infinite Prevention Example';

data mylib.out; 
    i = 1;
    do while (i <= 3);
        square = i * i;
        output; 
        /* Missing increment to prevent infinite loop */
    end;
run;

proc print data=mylib.out;
run;
