options linesize=80 pagesize=60;
libname mylib 'C:\Data';
title 'Many-to-Many Merge Example';

data mylib.out; 
    merge mylib.dataset1 mylib.dataset2;
    by id;
    output; 
run;

proc print data=mylib.out;
run;
