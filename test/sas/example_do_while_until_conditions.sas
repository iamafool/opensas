options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'DO WHILE and DO UNTIL Conditions Example';

data mylib.out; 
    /* DO WHILE loop */
    i = 1;
    do while (i <= 3);
        square = i * i;
        output; 
        i + 1;
    end;

    /* DO UNTIL loop */
    j = 1;
    do until (j > 3);
        cube = j * j * j;
        output; 
        j + 1;
    end;
run;

proc print data=mylib.out;
run;
