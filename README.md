# getliner

NB HUGE BUG, must +1 when using -l and zipped files. Sorry for inconveincace. (tsk)


This is a general tool for extract lines from newline seperated textfiles (can be gz compressed). Getliners4 allows for complement grep, use '-v 1'. Default is '-v 0'. Program can also print out basic statistics for hit/nonhit if -i filename is supplied. getliners6.cpp tokinzes on \r, so should work on windows files.



g++ getliners.cpp -lz -O3 -o getliner


Program can either extract specific lines, or "keys". The linenumbers to extract must be 1-indexed (first line i 1). When using keys, you must supply which column to 'grep' for. The program builds an associative array of the keys. And therefore only a single pass of the datafile is required.


Fields in the datafile can be seperated by a delimiter that can be specified at runtine with the "-d " argument. Default is space/tab. If delimter is an escape sequence it will not work and the code should be modified.


