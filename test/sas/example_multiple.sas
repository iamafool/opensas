options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Multiple Data Steps Example';

data mylib.out1;     set mylib.in1;     x = x * 2;     output; run;

data mylib.out2;     set mylib.in2;     y = y + 5;     if y > 25 then output; run;
