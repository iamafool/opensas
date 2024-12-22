options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'ELSE IF with Blocks Example';

data mylib.out; 
    set mylib.in; 
    if x > 15 then do;
        category = 'Very High';
        y = y * 3;
        z = y + 5;
    end;
    else if x > 10 then do;
        category = 'High';
        y = y * 2;
        z = y + 3;
    end;
    else if x > 5 then do;
        category = 'Medium';
        y = y + 10;
        z = y + 1;
    end;
    else do;
        category = 'Low';
        y = y + 5;
        z = y + 0;
    end;
    output; 
run;

proc print data=mylib.out;
run;
