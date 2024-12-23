options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Basic PROC MEANS Example';

proc means data=mylib.sales;
    var revenue profit;
    run;

proc print data=mylib.sales_means;
    run;
