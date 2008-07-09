#!/bin/sh

TESTFILES='alphabet-26.txt
	   c8-10000.txt
	   sv-17415.txt
	   ansiC-32.txt
	   nngs-4043.txt
	   xxx-17576.txt'

echo
echo Fast

for f in $TESTFILES ; do

    echo
    echo $f
    ../htabtest < $f

done

echo
echo Good

for f in $TESTFILES ; do

    echo
    echo $f
    ../htabtest -g < $f

done
