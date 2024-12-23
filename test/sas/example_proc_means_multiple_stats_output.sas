options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'PROC MEANS with Multiple Statistics and OUTPUT Statement Example';

proc means data=mylib.sales n mean median std min max;
    var revenue profit;
    output out=mylib.sales_stats n=N mean=Mean median=Median std=StdDev min=Min max=Max;
    run;

proc print data=mylib.sales_stats;
    run;
