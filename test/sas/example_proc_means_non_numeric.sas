options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS with Non-Numeric Variables Example';

proc means data=mylib.sales n mean;
    var revenue profit name; /* 'name' is non-numeric */
    run;

proc print data=mylib.sales_means;
    run;
