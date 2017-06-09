#!/bin/bash
for i in `seq -w 1 22`;
do
        sed -n -e '1p' -e '6p' -e '9p' -e '12p' out/${i}
done
