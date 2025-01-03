data in;
    input x num1 num2 num3;
    datalines;
1 5 10 15
2 10 15 20
3 15 20 25
;
run;

data out; 
    set in; 
    retain sum 0;
    array nums {3} num1 num2 num3;
    do i = 1 to 3;
        nums{i} = nums{i} + 10;
        sum = sum + nums{i};
    end;
    drop i;
    keep x sum num1 num2 num3;
    if sum > 25 then output; 
run;

proc print data=out;
run;