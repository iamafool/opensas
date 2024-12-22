options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Additional Data Step Features Example';

data mylib.out; 
    set mylib.in; 
    retain total 0;
    array scores {3} score1 score2 score3;
    do i = 1 to 3;
        scores{i} = scores{i} + 5;
        total = total + scores{i};
    end;
    drop i;
    keep x total score1 score2 score3;
    if total > 20 then output; 
run;

proc print data=mylib.out;
run;