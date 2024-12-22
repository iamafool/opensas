options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Nested ELSE IF Example';

data mylib.out; 
    set mylib.in; 
    if x > 15 then do;
        category = 'Very High';
        if y > 30 then status = 'Excellent';
        else status = 'Good';
    end;
    else if x > 10 then do;
        category = 'High';
        if y > 20 then status = 'Good';
        else status = 'Fair';
    end;
    else if x > 5 then do;
        category = 'Medium';
        status = 'Average';
    end;
    else do;
        category = 'Low';
        status = 'Poor';
    end;
    output; 
run;

proc print data=mylib.out;
run;
