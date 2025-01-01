data in;
    input x y;
    datalines;
5 10
15 20
10 15
20 25
;
run;

data out; 
    set in; 
    if x > 10 then do;
        status = 'High';
        y = y * 2;
    end;
    else do;
        status = 'Low';
        y = y + 5;
    end;
    output; 
run;

proc print data=out;
run;
