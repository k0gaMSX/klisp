#!/bin/bash

in=/tmp/$$.1
out=/tmp/$$.2

for i in test/*.l; do
    printf "Running test $i "
    awk '// {if (out == 1) print} /Return/ {out=1}' < $i | sed 's/;//' > $out
    ./kemacs < $i > $in 2> /dev/null 
    diff -w $in $out > /dev/null 2>&1
    if [ $? -ne 0 ]; then
       	echo [FAILED]
    else
       echo [OK]
    fi
    rm $in $out
done
