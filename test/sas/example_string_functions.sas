data in;
    length name $40;
    name="Alice"; output;
    name="  Bob  "; output;
    name="Charlie  "; output;
    name="Dana"; output;
run;

data out; 
    set in; 
    first_part = substr(name, 1, 3);
    trimmed = trim(name);
    upper_name = upcase(name);
    lower_name = lowcase(name);
    output; 
run;

proc print data=out;
run;
