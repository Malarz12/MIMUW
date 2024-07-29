#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments>"
    exit 1
fi

number=$1

SRC1="./trz_brute_force.c"
SRC2="./trz.c"
SRC3="./generate_input.cpp"

PROGRAM1="./trz_brute_force.e"
PROGRAM2="./trz.e"
PROGRAM3="./generate_input.e"

OUTPUT1="output1.txt"
OUTPUT2="output2.txt"
INPUT="input.txt"

gcc @opcje $SRC1 -o $PROGRAM1
gcc @opcje $SRC2 -o $PROGRAM2
g++ $SRC3 -o $PROGRAM3

for i in $(seq 1 $number); do
    
    $PROGRAM3 > $INPUT
    input=$(<"$INPUT")

    echo $input | $PROGRAM1 > $OUTPUT1
    echo $input | $PROGRAM2 > $OUTPUT2

    if diff -q $OUTPUT1 $OUTPUT2 > /dev/null; then
        echo "TEST nr. $i has PASSED"
    else
        echo "TEST nr. $i has NOT PASSED"
        cat $INPUT
        echo "trz_brute_force: "
        cat $OUTPUT1
        echo "trz: "
        cat $OUTPUT2
    fi
done

rm -f $OUTPUT1 $OUTPUT2
