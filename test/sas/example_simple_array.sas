options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Simple Array Example';

data mylib.out; 
    set mylib.in; 
    array scores{3} score1 score2 score3;
    do i = 1 to 3;
        scores{i} = scores{i} + 10;
    end;
    output; 
run;

proc print data=mylib.out;
run;