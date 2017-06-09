#!/bin/bash
for i in `seq -w 1 22`;
do
	ktest-tool --write-ints klee-last/test0000${i}.ktest > out/${i}
done
